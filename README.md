# LilyGo T-Panel S3 Lite + openHASP

Notes from bringing up a **T-Panel S3 Lite** (no touch) on [openHASP](https://github.com/HASwitchPlate/openHASP).

Firmware **boots**: `TFT : Started`, `LVGL: Started`, `OOBE: h: 480 - v: 480`. Confirm the panel image on hardware; if the screen is dark, try backlight GPIO 14 (wiki) instead of 33 (LilyGO source).

## This is not the full T-Panel

| | T-Panel S3 | T-Panel S3 Lite |
|---|---|---|
| Touch | CST3240 | none |
| IO expander | XL9535 (SPI init + reset) | **none** — SPI on ESP32 GPIOs |
| Coprocessor | ESP32-H2 | none |
| PSRAM | check schematic | **8 MB Quad SPI**, not Octal |
| Backlight | GPIO14 | GPIO **33** (source) |

Do **not** copy the regular T-Panel env (`LILYGO_T_PANEL` + `Arduino_XL9535SWSPI`). RGB data pins happen to match; SPI, DE, backlight, and PSRAM mode do not.

## Pinout (from LilyGO source)

Canonical file: [`t_panel_config.h`](https://github.com/Xinyuan-LilyGO/T-Panel-Lite/blob/main/libraries/private_library/t_panel_config.h).

| Signal | GPIO |
|---|---|
| SPI CS / SCLK / MOSI (9-bit, DC unused) | 14 / 36 / 35 |
| DE / HSYNC / VSYNC / PCLK | 38 / 39 / 40 / 41 |
| B0–B4 | 1, 2, 3, 4, 5 |
| G0–G5 | 6–11 |
| R0–R4 | 12, 13, 42, 46, 45 |
| Backlight | 33 |
| SD CS / SCLK / MOSI / MISO | 34 / 36 / 35 / 37 |
| KEY1 / KEY2 / BOOT | 48 / 47 / 0 |

RGB timing (same header): 6 MHz, DE mode, hsync 20/1/1, vsync 30/1/10, `pclk_active_neg = 0`, idle-high sync. Arduino_GFX 1.4.7 encodes idle-high as **polarity = 1** (`hsync_idle_low = (polarity == 0)`).

## Wiki vs source

[LilyGO wiki: T-Panel S3 Lite](https://wiki.lilygo.cc/products/t-panel-series/t-panel-s3-lite/) is useful but wrong in places:

- Claims **XL9535**, then lists ESP32 GPIOs for SPI CS/SCLK/MOSI. Lite source and schematic have **no expander**.
- Does not list **DE** (GPIO 38).
- Lists backlight as **GPIO14**, same pin as SPI CS. Source uses **GPIO33**. 14 as BL would also be CS after init (held high → backlight on). Unverified on our board.

RGB data pins on the wiki match the source.

## PSRAM: Quad, not Octal

Official `sdkconfig.defaults`: `CONFIG_SPIRAM_MODE_QUAD`. GPIO 33–37 are BL / SD / SPI. **Octal OPI** (`qio_opi`, `N16R8`, `CONFIG_SPIRAM_MODE_OCT`) takes those pins and crashes RGB/SPI init.

Use `qio_qspi` / Quad PSRAM. Arduino_GFX 1.4.7 then defaults the RGB clock to 6 MHz, which matches LilyGO.

## Arduino_GFX data pin order

`Arduino_ESP32RGBPanel` (v1.4.7, little-endian) maps constructor `b0..b4` to `data_gpio_nums[0..]`. LilyGO’s `esp_lcd` config puts **R0 first**. To match that, pass **B, G, R** into the constructor (same trick as openHASP’s regular T-Panel path). Passing R,G,B by name would swap red/blue on the bus.

Init sequence: existing `st7701_t_panel_init_operations` (same YDP395BT001 panel as T-Panel).

## Flash over USB JTAG

Chip enumerates as Espressif USB JTAG (`303A:1001`).

- 921600 / default RTS reset often drops the port mid-write.
- Works: `--before usb_reset` (and 115200 is fine; baud is not the issue).
- After a failed write, `erase_flash` then `write_flash` of the merged image at `0x0`.

```bash
esptool.py --chip esp32s3 --port /dev/cu.usbmodem1101 --baud 115200 \
  --before usb_reset --after no_reset erase_flash

esptool.py --chip esp32s3 --port /dev/cu.usbmodem1101 --baud 115200 \
  --before no_reset --after hard_reset write_flash --flash_mode keep --flash_size keep \
  0x0 lilygo-t-panel-s3-lite_full_16MB.bin
```

## Using these files with openHASP

Copy onto an openHASP tree:

```
openhasp/boards/lilygo-t-panel-s3-lite.json
  → boards/lilygo-t-panel-s3-lite.json

openhasp/user_setups/esp32s3/lilygo-t-panel-s3-lite.ini
  → user_setups/esp32s3/lilygo-t-panel-s3-lite.ini
```

Enable the env (openHASP does not load `user_setups/esp32s3/*.ini` unless listed in `platformio_override.ini` extra_configs). Or paste the env into `platformio_override.ini`.

Insert the Lite `#elif` from `openhasp/src/drv/tft/t_panel_lite_init.cpp` into `src/drv/tft/tft_driver_arduinogfx.cpp` **before** the `LILYGO_T_PANEL` branch.

No-touch: `-D TOUCH_DRIVER=-1`. Guard `guiSetup()` so it does not touch `indev_drv` when no pointer device is registered.

## Status

Tested 2026-09-02 on a Lite, USB JTAG, openHASP 0.7.0.1 + Arduino-ESP32 2.0.14 + Arduino_GFX 1.4.7 (Tasmota platform 2023.10.03).

After a full erase, device came up in AP `HASP-xxxxxx` at `http://192.168.4.1`.
