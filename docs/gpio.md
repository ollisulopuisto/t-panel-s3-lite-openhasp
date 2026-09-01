# GPIO and buttons

Lite hardware buttons (LilyGO `t_panel_config.h`):

| Button | GPIO | openHASP group | Type |
|---|---|---|---|
| BOOT | 0 | 1 | `BUTTON_TYPE`, `INTERNAL_PULLUP` |
| KEY2 | 47 | 2 | `BUTTON_TYPE`, `INTERNAL_PULLUP` |
| KEY1 | 48 | 3 | `BUTTON_TYPE`, `INTERNAL_PULLUP` |

Firmware defaults these in `gpioConfig` and `gpioSetup()` re-applies them if saved config has none (so an older NVS without GPIO 48 still gets KEY1).

Boot log:

```
GPIO: Saving Pin config #0 pin 0 - type 240 - group 1
GPIO: Saving Pin config #1 pin 47 - type 240 - group 2
GPIO: Saving Pin config #2 pin 48 - type 240 - group 3
```

`type 240` is `BUTTON_TYPE`. Home Assistant discovery turns them into **event entities**, not screen objects. Use automations on those events. There is no touchscreen; do not bind `event:` on page objects for taps.

## Why GPIO 48 was missing from the HTTP UI

Arduino-ESP32’s S3 variant sets `NUM_DIGITAL_PINS` to **48**, i.e. GPIO **0..47**. The chip has GPIO **48**.

openHASP’s GPIO picker looped `for (gpio = 0; gpio < NUM_DIGITAL_PINS)`. KEY1 never appeared.

Fix: `HASP_GPIO_PIN_COUNT` is 49 on ESP32-S3, and the HTTP GPIO loops use that instead of `NUM_DIGITAL_PINS`. See [patches](patches.md).

`is_system_pin` still treats S3 GPIO 33–37 as octal-PSRAM reserved, so backlight / SD / SPI stay hidden from the picker. That is correct on Lite: those pins are used.

## Rediscovery

If Home Assistant already discovered the plate **before** the buttons were in firmware, remove the device and let MQTT discovery run again so the event entities appear.
