# LilyGo T-Panel S3 Lite + openHASP

Bring-up notes and overlay files for a **T-Panel S3 Lite** (no touch) on [openHASP](https://github.com/HASwitchPlate/openHASP) 0.7.x.

**Verified 2026-09-02** on hardware: TFT/LVGL 480×480, WiFi, HTTP, MQTT, Home Assistant discovery, three hardware buttons, boot from a 5 V USB-C brick. Backlight is **GPIO 33** (LilyGO source), not wiki GPIO 14.

`ARDUINO_USB_CDC_ON_BOOT` must be **0** or the plate will not boot on a power brick (it waits for a USB host). See [power](docs/power.md).

Upstream PR: [HASwitchPlate/openHASP#1037](https://github.com/HASwitchPlate/openHASP/pull/1037).

## Docs

| | |
|---|---|
| [Flash over USB JTAG](docs/flash.md) | `esptool` flags that actually work |
| [GPIO and buttons](docs/gpio.md) | BOOT / KEY2 / KEY1, why GPIO 48 was missing |
| [Display and bezel](docs/display.md) | 480×480 square panel, round overlay |
| [Home Assistant](docs/home-assistant.md) | MQTT, pages.jsonl, no-touch objects |
| [Power](docs/power.md) | 5 V USB-C; `CDC_ON_BOOT=0` or a brick never boots |
| [Source patches](docs/patches.md) | Overlay files and the remaining edits |

## This is not the full T-Panel

| | T-Panel S3 | T-Panel S3 Lite |
|---|---|---|
| Touch | CST3240 | **none** |
| IO expander | XL9535 (SPI init + reset) | **none** — SPI on ESP32 GPIOs |
| Coprocessor | ESP32-H2 | none |
| PSRAM | check schematic | **8 MB Quad SPI**, not Octal |
| Backlight | GPIO14 | GPIO **33** (confirmed) |
| Power | 7–24 V barrel + USB | **5 V USB-C only** |

Do **not** copy the regular T-Panel env (`LILYGO_T_PANEL` + `Arduino_XL9535SWSPI`). RGB data pins happen to match; SPI, DE, backlight, PSRAM mode, and power do not.

## Pinout (from LilyGO source)

Canonical file: [`t_panel_config.h`](https://github.com/Xinyuan-LilyGO/T-Panel-Lite/blob/main/libraries/private_library/t_panel_config.h).

| Signal | GPIO |
|---|---|
| SPI CS / SCLK / MOSI (9-bit, DC unused) | 14 / 36 / 35 |
| DE / HSYNC / VSYNC / PCLK | 38 / 39 / 40 / 41 |
| B0–B4 | 1, 2, 3, 4, 5 |
| G0–G5 | 6–11 |
| R0–R4 | 12, 13, 42, 46, 45 |
| Backlight | **33** |
| SD CS / SCLK / MOSI / MISO | 34 / 36 / 35 / 37 |
| KEY1 / KEY2 / BOOT | 48 / 47 / 0 |

RGB timing (same header): 6 MHz, DE mode, hsync 20/1/1, vsync 30/1/10, `pclk_active_neg = 0`, idle-high sync. Arduino_GFX 1.4.7 encodes idle-high as **polarity = 1** (`hsync_idle_low = (polarity == 0)`).

## Wiki vs source

[LilyGO wiki: T-Panel S3 Lite](https://wiki.lilygo.cc/products/t-panel-series/t-panel-s3-lite/) is useful but wrong in places:

- Claims **XL9535**, then lists ESP32 GPIOs for SPI CS/SCLK/MOSI. Lite source and schematic have **no expander**.
- Does not list **DE** (GPIO 38).
- Lists backlight as **GPIO14**, same pin as SPI CS. Source uses **GPIO33**. Confirmed working on our board.

RGB data pins on the wiki match the source.

## PSRAM: Quad, not Octal

Official `sdkconfig.defaults`: `CONFIG_SPIRAM_MODE_QUAD`. GPIO 33–37 are BL / SD / SPI. **Octal OPI** (`qio_opi`, `N16R8`, `CONFIG_SPIRAM_MODE_OCT`) takes those pins and crashes RGB/SPI init.

Use `qio_qspi` / Quad PSRAM. Arduino_GFX 1.4.7 then defaults the RGB clock to 6 MHz, which matches LilyGO.

## Arduino_GFX data pin order

`Arduino_ESP32RGBPanel` (v1.4.7, little-endian) maps constructor `b0..b4` to `data_gpio_nums[0..]`. LilyGO’s `esp_lcd` config puts **R0 first**. To match that, pass **B, G, R** into the constructor (same trick as openHASP’s regular T-Panel path). Passing R,G,B by name would swap red/blue on the bus.

Init sequence: existing `st7701_t_panel_init_operations` (same YDP395BT001 panel as T-Panel).

## Using these files with openHASP

Copy onto an openHASP tree (0.7.x, Arduino-ESP32 2.0.14, Arduino_GFX 1.4.7, Tasmota platform 2023.10.03):

```
openhasp/boards/lilygo-t-panel-s3-lite.json
  → boards/lilygo-t-panel-s3-lite.json

openhasp/user_setups/esp32s3/lilygo-t-panel-s3-lite.ini
  → user_setups/esp32s3/lilygo-t-panel-s3-lite.ini
```

Enable the env (openHASP does not load `user_setups/esp32s3/*.ini` unless listed in `platformio_override.ini` extra_configs). Or paste the env into `platformio_override.ini`.

Insert the Lite `#elif` from `openhasp/src/drv/tft/t_panel_lite_init.cpp` into `src/drv/tft/tft_driver_arduinogfx.cpp` **before** the `LILYGO_T_PANEL` branch.

Then apply [the remaining patches](docs/patches.md): GPIO 48 in the HTTP UI, default buttons, no-touch `guiSetup()`.

## Status

Tested 2026-09-02: USB JTAG (`303A:1001`), openHASP 0.7.0.1.

After a full erase, device came up in AP `HASP-xxxxxx` at `http://192.168.4.1`. After WiFi: HTTP + MQTT; Home Assistant openHASP 0.7.x discovery worked. With `CDC_ON_BOOT=0` it also boots from a 5 V USB-C brick. USB to a computer is then only needed for flashing.
