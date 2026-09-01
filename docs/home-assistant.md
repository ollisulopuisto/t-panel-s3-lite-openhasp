# Home Assistant

Match firmware 0.7.x with the HACS [openHASP custom component](https://github.com/HASwitchPlate/openHASP-custom-component) 0.7.x.

## MQTT on the plate

WiFi first (HTTP at the AP `http://192.168.4.1`, or later at the station IP). Then MQTT:

| Field | Example |
|---|---|
| Node / hostname | `tpanel` (unique) |
| Broker | Home Assistant Mosquitto IP |
| Port | 1883 |
| User / pass | whatever the broker requires |

Serial (USB still plugged in):

```
hostname tpanel
mqtthost 192.168.1.202
mqttport 1883
mqttuser <user>
mqttpass <pass>
reboot
```

LWT topic becomes `hasp/tpanel/LWT`. `MQTT: Connection refused: Authentication error` means the broker is reachable but user/pass is wrong.

Once MQTT is up, USB is optional. Config stays on HTTP/MQTT.

## Discovery

With MQTT discovery enabled, the plate appears under Devices & Services as an openHASP device. Slug is the node name (`tpanel`).

## Screen without touch

`pages.jsonl` is layout (`pXbY` = page X, id Y). Home Assistant binds **properties only** — no `event:` for taps.

```yaml
# configuration.yaml
openhasp:
  tpanel:
    objects:
      - obj: "p1b10"
        properties:
          "text": '{{ states("sensor.outdoor_temperature") }}°C'
      - obj: "p1b20"
        properties:
          "val": '{{ 1 if is_state("light.living_room", "on") else 0 }}'
```

See [`examples/openhasp.yaml`](../examples/openhasp.yaml) and [`examples/pages.jsonl`](../examples/pages.jsonl). Official object docs: [openHASP pages](https://www.openhasp.com/latest/design/objects/).

## Buttons

BOOT / KEY2 / KEY1 show up as event entities. Drive automations from those, not from on-screen `btn` objects, unless you add on-screen widgets that HA updates (still no tap).
