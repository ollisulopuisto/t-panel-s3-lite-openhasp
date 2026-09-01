# Source patches

Against openHASP 0.7.x (`d1a0770` and nearby). Board JSON and the PlatformIO env are drop-in copies. The rest is surgical.

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

`-D LILYGO_T_PANEL_LITE=1` and `-D TOUCH_DRIVER=-1` are already in the env.

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

In `gpioSetup()`, after `aceButtonSetup()`, re-apply if NVS had none:

```c
#if defined(LILYGO_T_PANEL_LITE)
    {
        const uint8_t lite_btns[] = {0, 47, 48}; // BOOT, KEY2, KEY1
        for(uint8_t b = 0; b < 3; b++) {
            bool found = false;
            for(uint8_t i = 0; i < HASP_NUM_GPIO_CONFIG; i++) {
                if(gpioConfig[i].pin == lite_btns[b] && gpioConfig[i].type == hasp_gpio_type_t::BUTTON_TYPE) {
                    found = true;
                    break;
                }
            }
            if(!found) {
                int8_t id = gpioGetFreeConfigId();
                if(id >= 0) {
                    gpioSavePinConfig(id, lite_btns[b], hasp_gpio_type_t::BUTTON_TYPE, (uint8_t)(b + 1),
                                      hasp_gpio_function_t::INTERNAL_PULLUP, false);
                }
            }
        }
    }
#endif
```

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
