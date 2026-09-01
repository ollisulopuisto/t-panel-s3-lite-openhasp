# Flash over USB JTAG

Chip enumerates as Espressif USB JTAG (`303A:1001`). On macOS the port is typically `/dev/cu.usbmodem1101`.

## What fails

- 921600 / default RTS reset often drops the port mid-write.
- Fast `esptool` with the PlatformIO default `--before default_reset` is unreliable on this CDC/JTAG.

## What works

`--before usb_reset` at 115200. Baud is not the issue; the reset mode is.

```bash
# First flash or wipe
esptool.py --chip esp32s3 --port /dev/cu.usbmodem1101 --baud 115200 \
  --before usb_reset --after no_reset erase_flash

esptool.py --chip esp32s3 --port /dev/cu.usbmodem1101 --baud 115200 \
  --before no_reset --after hard_reset write_flash --flash_mode keep --flash_size keep \
  0x0 lilygo-t-panel-s3-lite_full_16MB.bin
```

App-only update (keeps NVS / WiFi / MQTT):

```bash
esptool.py --chip esp32s3 --port /dev/cu.usbmodem1101 --baud 115200 \
  --before usb_reset --after hard_reset write_flash --flash_mode keep --flash_size keep \
  0x10000 firmware.bin
```

`firmware.bin` is `.pio/build/lilygo-t-panel-s3-lite/firmware.bin`. The merged image is `build_output/firmware/lilygo-t-panel-s3-lite_full_16MB_*.bin`.

## First boot after erase

Serial can stay silent until the port is open **and** RTS is pulsed. Open a serial session, then reset:

```python
import serial, time
ser = serial.Serial('/dev/cu.usbmodem1101', 115200, timeout=0.2)
ser.dtr = False
ser.rts = True
time.sleep(0.15)
ser.rts = False
```

After a failed write: `erase_flash` then `write_flash` of the merged image at `0x0`.

## After CDC on boot is off

Production firmware uses `ARDUINO_USB_CDC_ON_BOOT=0` so a 5 V brick can boot the plate. USB JTAG flashing still works (`ARDUINO_USB_MODE=1`). The USB serial log is quiet; use HTTP or MQTT.

## After it is on WiFi

OTA and HTTP (`http://<plate-ip>`) work. USB is not required unless you need another flash. See [power](power.md).
