# Source patches

Board JSON, the PlatformIO env, GPIO 48 in the HTTP picker, and no-touch `guiSetup()` landed in [openHASP#1037](https://github.com/HASwitchPlate/openHASP/pull/1037). Overlay copies remain for 0.7.x trees that predate that merge.

The rest is surgical. One piece is **not** upstream: NVS re-apply of BOOT/KEY2/KEY1.

## Copy as-is

```
openhasp/boards/lilygo-t-panel-s3-lite.json
  → boards/lilygo-t-panel-s3-lite.json

openhasp/user_setups/esp32s3/lilygo-t-panel-s3-lite.ini
  → user_setups/esp32s3/lilygo-t-panel-s3-lite.ini
```

In `platformio_override.ini`:

```ini
[platformio]
extra_configs =
    user_setups/esp32s3/lilygo-t-panel-s3-lite.ini
```

Or paste the `[env:lilygo-t-panel-s3-lite]` section into the override.

## TFT init

Insert `openhasp/src/drv/tft/t_panel_lite_init.cpp` into `src/drv/tft/tft_driver_arduinogfx.cpp` **before** the `#elif … LILYGO_T_PANEL` branch. The Lite `#elif` must win over the full T-Panel XL9535 path.

`-D LILYGO_T_PANEL_LITE=1` and `-D TOUCH_DRIVER=-1` are already in the env. Keep `ARDUINO_USB_CDC_ON_BOOT=0` (a brick will not boot if CDC waits for a host). `ARDUINO_USB_MODE=1` stays so USB JTAG flashing still works.

## GPIO 48 in the HTTP UI

`src/sys/gpio/hasp_gpio.h` — after `gpioConfig` extern:

```c
#ifndef HASP_GPIO_PIN_COUNT
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define HASP_GPIO_PIN_COUNT 49
#else
#define HASP_GPIO_PIN_COUNT NUM_DIGITAL_PINS
#endif
#endif
```

`src/sys/svc/hasp_http.cpp` and `hasp_http_async.cpp`: replace GPIO picker loops

```c
for(uint8_t gpio = 0; gpio < NUM_DIGITAL_PINS; gpio++)
```

with `HASP_GPIO_PIN_COUNT`. Same for `io` loops in the GPIO input/output handlers.

## Default buttons

`src/sys/gpio/hasp_gpio.cpp` `gpioConfig[]`:

```c
#elif defined(LILYGO_T_PANEL_LITE)
    {.pin = 0, .group = 1, .gpio_function = INTERNAL_PULLUP, .type = BUTTON_TYPE},  // BOOT
    {.pin = 47, .group = 2, .gpio_function = INTERNAL_PULLUP, .type = BUTTON_TYPE}, // KEY2
    {.pin = 48, .group = 3, .gpio_function = INTERNAL_PULLUP, .type = BUTTON_TYPE}, // KEY1
#endif
```

In `gpioSetup()`, after `aceButtonSetup()`, re-apply if NVS had none. Copy from `openhasp/src/sys/gpio/t_panel_lite_nvs_buttons.cpp` (not in #1037).

## No-touch guiSetup

`src/hasp_gui.cpp`: the `indev_drv.long_press_*` NVS reads sit **outside** `#if TOUCH_DRIVER != -1`. With `TOUCH_DRIVER=-1`, `indev_drv` does not exist.

Wrap that ESP32 Preferences block:

```c
#if defined(ARDUINO_ARCH_ESP32)
#if TOUCH_DRIVER != 0
    Preferences preferences;
    nvs_user_begin(preferences, "gui", true);
    // ...
    preferences.end();
#endif
```

(`TOUCH_DRIVER != 0` is enough here: Lite is `-1`.)
