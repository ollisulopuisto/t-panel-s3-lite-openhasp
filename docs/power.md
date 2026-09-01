# Power

Lite is **5 V USB-C only**. The full T-Panel’s 7–24 V barrel jack is not on this board.

LilyGO does not publish a Lite current draw. With screen + WiFi, expect roughly **200–400 mA / 1–2 W**.

| Source | |
|---|---|
| 5 V / 1 A USB-C brick | enough |
| 5 V / 2 A | comfortable |
| PC USB 2.0 (500 mA) | can brown out on WiFi TX |

## USB after bring-up

Once WiFi and MQTT work, the computer USB cable is **not required**. Plug into a 5 V USB-C brick and leave it.

Reconnect USB only to:

- flash firmware
- watch the serial console
- recover a brick (erase + merged image)

HTTP at the plate IP and MQTT stay available on WiFi.
