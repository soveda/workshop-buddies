# Buzzrito Modal WebUSB Editor Experiment

**Stages 1-2 passed:** normal performance remains stable, the original
Buzzrito web app connects reliably in Switch-Up editor boot mode, and edited
preset data survives reboot in the card's internal flash.

This is the active experimental fallback. Parameter-restoration work branches
from it and must not replace it.

This is a safe WebUSB connection test based on the passed modal USB boot
architecture. It reuses the original Buzzrito vendor endpoint, VID family, and
`0x0b47` packet format used by the Buzzrito web app.

## Behaviour

- Boot with the switch in Middle: normal, stable Workshop Buzzrito performance.
  USB is not initialized.
- Boot with the latched switch Up: muted WebUSB editor mode. The card presents
  `Workshop Buzzrito WebUSB` and its vendor interface for the existing web app.
- Power-cycle with the switch in Middle to leave editor mode.

The editor accepts and replies to the original preset packets. To save edited
presets, move the latched switch to **Down** and hold it for one second. The
card mutes, writes one CRC-checked preset record to its reserved final flash
sector, then reboots into normal performance mode. The saved presets load on
every later boot, including ordinary non-USB performance boots.

Keeping editor mode separate is deliberate: it prevents USB traffic and flash
writes from disturbing audio or gesture playback.

## Audible Parameters

Normal performance always takes X and Y from the physical knobs or their CV
inputs; the browser's virtual XY position is editor-only. The stable Workshop
renderer uses saved `spread`, `wobble_amount`, `wobble_speed`, `saw_level`,
`sub_level`, `comb_depth`, and `comb_mul` values. It deliberately does not add
the original pink-noise path, and keeps the original motion-generating behavior
disabled for stable knob operation. A stored value can therefore be visible in
the app without producing the original firmware's exact audible result.

## Reset

The original web app can write its default preset values back to the editor.
Save that state with the same one-second Switch-Down hold to make those defaults
the card's new stored presets.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
```

The persistence test artifact is
`uf2/workshop_buzzrito_webusb_modal_editor_persistence.uf2`.
