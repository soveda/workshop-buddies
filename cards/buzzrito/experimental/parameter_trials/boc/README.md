# Buzzrito BOC Test

This isolated test derives from the hardware-passed glide version. It adds only
the original `boc_amount` path; the glide version remains unchanged.

**Hardware passed:** `boc_amount = 0` retains the glide baseline, restrained
BOC movement is stable at the trial's capped maximum, and motion, Pulse2, gate,
LED, and WebUSB-editor behavior remain intact.

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

## BOC Behaviour

The original BOC interpolation-noise generator advances at its original
64-sample cadence: 750 updates per second, not once on every 48 kHz
`ProcessSample()` callback. Its value is held between updates. The shared BOC
drift reaches the sub and comb pitch paths at full depth and the saws at half
depth, as in the original firmware.

This first trial limits `boc_amount` to one eighth of its original range. It
should provide restrained shared pitch movement without fast modulation or
unstable oscillator behavior.

## Audible Parameters

Normal performance always takes X and Y from the physical knobs or their CV
inputs; the browser's virtual XY position is editor-only. The stable Workshop
renderer uses saved `spread`, `glide`, `boc_amount`, `wobble_amount`, `wobble_speed`,
`saw_level`, `sub_level`, `comb_depth`, and `comb_mul` values. It deliberately
does not add the original pink-noise path, and keeps the original
motion-generating behavior disabled for stable knob operation. In the renderer,
`noise_level` is locked to zero. The web app still displays and saves that
field, but changing it has no audible effect.

## Reset

The original web app can write its default preset values back to the editor.
Save that state with the same one-second Switch-Down hold to make those defaults
the card's new stored presets.

## Build

```sh
cmake -S . -B /private/tmp/workshop-buzzrito-boc -DCMAKE_BUILD_TYPE=Release
cmake --build /private/tmp/workshop-buzzrito-boc -j2
```

The generated test UF2 is intentionally not stored in Git. It is written to
the ignored `cards/buzzrito/test-artifacts/` directory when built for hardware
testing.
