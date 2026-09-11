# Buzzrito Half-Depth BOC Calibration

This isolated test derives from the hardware-passed noise version. It changes
only the BOC cap from one eighth to one half of the original range.

The inherited glide, one-eighth BOC, and capped-noise behavior has passed
hardware testing. This half-depth BOC calibration itself requires testing.

The original Buzzrito web app can edit and store `boc_amount` values in this build.
It retains the existing vendor endpoint, VID family, and `0x0b47` packet
format.

## Behaviour

- Boot with the switch in Middle: normal, stable Workshop Buzzrito performance.
  USB is not initialized.
- Boot with the latched switch Up: muted WebUSB editor mode. The card presents
  `Workshop Buzzrito WebUSB` and its vendor interface for the existing web app.
- Power-cycle with the switch in Middle to leave editor mode.

The editor accepts and replies to the original preset packets. To save edited
presets, move the latched switch to **Down** and hold it for one second. The
card mutes, writes one CRC-checked preset record to its reserved final flash
sector, then reboots into normal performance mode.

Keeping editor mode separate is deliberate: it prevents USB traffic and flash
writes from disturbing audio or gesture playback.

## BOC Calibration

BOC interpolation noise advances at the original 64-sample cadence, with the
original unscaled `wobble_speed`. Its shared drift reaches sub/comb at full
depth and saws at half depth. This calibration raises the BOC cap from `1/8`
to `1/2`; pink noise remains at its passed `1/16` cap.

At `boc_amount = 0`, this build preserves the passed noise baseline.

## Audible Parameters

Normal performance always takes X and Y from the physical knobs or their CV
inputs; the browser's virtual XY position is editor-only. The stable Workshop
renderer uses saved `spread`, `glide`, `boc_amount`, `wobble_amount`, `wobble_speed`,
`saw_level`, `sub_level`, `noise_level`, `comb_depth`, and `comb_mul` values. It deliberately
adds the passed depth-limited pink-noise path, and keeps the original
motion-generating behavior disabled for stable knob operation.

## Reset

The original web app can write its default preset values back to the editor.
Save that state with the same one-second Switch-Down hold to make those defaults
the card's new stored presets.

## Build

```sh
cmake -S . -B /private/tmp/workshop-buzzrito-boc-half -DCMAKE_BUILD_TYPE=Release
cmake --build /private/tmp/workshop-buzzrito-boc-half -j2
```

The generated test UF2 is intentionally not stored in Git. It is written to
the ignored `cards/buzzrito/test-artifacts/` directory when built for hardware
testing.
