# Buzzrito Original Wobble Half-Depth Trial

This isolated test derives from the quarter-depth original-wobble trial. It
changes only the original per-saw wobble cap from one quarter to one half.

The inherited glide, full-depth BOC, and quarter-depth noise behavior has
passed hardware testing. The original quarter-depth wobble was too subtle;
this half-depth trial requires testing.

The original Buzzrito web app can edit and store `wobble_amount` and
`wobble_speed` values in this build.
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

## Half-Depth Original Wobble Trial

Each of the 16 saws has the original independent seeded pink/interpolation
state. Its state advances at the original 64-sample cadence, 750 times per
second, with the original unscaled `wobble_speed`. The deterministic sine
wobble is absent in this branch.

This trial caps `wobble_amount` at one half of the original range. At
`wobble_amount = 0`, it preserves the passed full-depth BOC and quarter-noise
baseline.

## Audible Parameters

Normal performance always takes X and Y from the physical knobs or their CV
inputs; the browser's virtual XY position is editor-only. The stable Workshop
renderer uses saved `spread`, `glide`, `boc_amount`, `wobble_amount`, `wobble_speed`,
`saw_level`, `sub_level`, `noise_level`, `comb_depth`, and `comb_mul` values. It deliberately
adds the passed quarter-depth pink-noise path, and keeps the original
motion-generating behavior disabled for stable knob operation.

## Reset

The original web app can write its default preset values back to the editor.
Save that state with the same one-second Switch-Down hold to make those defaults
the card's new stored presets.

## Build

```sh
cmake -S . -B /private/tmp/workshop-buzzrito-original-wobble-half -DCMAKE_BUILD_TYPE=Release
cmake --build /private/tmp/workshop-buzzrito-original-wobble-half -j2
```

The generated test UF2 is intentionally not stored in Git. It is written to
the ignored `cards/buzzrito/test-artifacts/` directory when built for hardware
testing.
