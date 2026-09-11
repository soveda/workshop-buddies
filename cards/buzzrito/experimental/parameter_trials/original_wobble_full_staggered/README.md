# Buzzrito Original Wobble Full-Depth Staggered Trial

This isolated test derives from the half-depth original-wobble trial. It
restores full original wobble depth and distributes the 16 per-saw target
updates over each 64-sample block.

The inherited glide, full-depth BOC, and quarter-depth noise behavior has
passed hardware testing. The original half-depth wobble exposed a fizzy,
distorted corner case at maximum X; the full-depth staggered update schedule
has passed that stress case without the failure occurring.

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

## Full-Depth Staggered Original Wobble

Each of the 16 saws has the original independent seeded pink/interpolation
state. Its state advances at the original 64-sample cadence, 750 times per
second, with the original unscaled `wobble_speed`. The deterministic sine
wobble is absent in this branch.

This trial restores the full `wobble_amount` range. Each saw still advances its
original seeded interpolation state exactly once per 64 samples at 750 Hz, but
each state/target update happens on a separate callback every four samples.
This avoids the previous burst of 16 interpolation and pitch-target updates in
one audio callback.

At `wobble_amount = 0`, it preserves the passed full-depth BOC and
quarter-noise baseline.

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
cmake -S . -B /private/tmp/workshop-buzzrito-original-wobble-full -DCMAKE_BUILD_TYPE=Release
cmake --build /private/tmp/workshop-buzzrito-original-wobble-full -j2
```

The generated test UF2 is intentionally not stored in Git. It is written to
the ignored `cards/buzzrito/test-artifacts/` directory when built for hardware
testing.
