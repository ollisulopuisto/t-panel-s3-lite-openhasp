# Display and bezel

The panel is a square **480×480** ST7701S (YDP395BT001), same glass as the full T-Panel. The round metal/plastic bezel is a **mechanical overlay**. Firmware must not apply a circular crop or origin offset.

Default `pages.jsonl` is a hostname label with no `x`/`y`, so it sits at (0,0) and the rounded corner clips it.

Keep content ~24 px off the corners:

```jsonl
{"page":1,"id":10,"x":24,"y":16,"w":432,"h":32,"obj":"label","text":"%hostname%"}
```

A full starter page is in [`examples/pages.jsonl`](../examples/pages.jsonl).

`First Touch Calibration enabled` in the boot log is default `calData`. Harmless with `TOUCH_DRIVER=-1`. There is no pointer device.

Upload `pages.jsonl` via the plate HTTP file editor, or MQTT `hasp/<node>/command/pages`.
