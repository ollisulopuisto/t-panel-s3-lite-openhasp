# Power

Lite is **5 V USB-C only**. The full T-Panel’s 7–24 V barrel jack is not on this board.

LilyGO does not publish a Lite current draw. With screen + WiFi, expect roughly **200–400 mA / 1–2 W**.

| Source | |
|---|---|
| 5 V / 1 A USB-C brick | enough |
| 5 V / 2 A | comfortable |
| PC USB 2.0 (500 mA) | can brown out on WiFi TX |

## USB CDC on boot

Firmware **must** ship with `ARDUINO_USB_CDC_ON_BOOT=0`.

With `=1`, `Serial` waits for a USB host. A computer enumerates CDC and the plate boots. A USB power brick never does, so the screen stays dark and WiFi never comes up. Same LilyGO note as [T-Display-S3](https://github.com/Xinyuan-LilyGO/T-Display-S3): disable CDC on boot for external power.

`ARDUINO_USB_MODE=1` stays on so USB JTAG flashing from a PC still works. After CDC-off, the USB serial log is quiet; HTTP and MQTT are the console.

The full T-Panel env already uses `CDC_ON_BOOT=0`.

## USB after bring-up

Once WiFi and MQTT work, the computer USB cable is **not required**. Plug into a 5 V USB-C brick and leave it.

Reconnect USB only to:

- flash firmware
- recover a brick (erase + merged image)

HTTP at the plate IP and MQTT stay available on WiFi.
