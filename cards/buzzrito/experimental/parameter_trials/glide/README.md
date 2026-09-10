# Buzzrito Glide Test

This isolated test derives from the passed modal WebUSB fallback. It adds only
the original `glide` parameter; the fallback source remains unchanged.

The original Buzzrito web app can edit and store `glide` values in this build.
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

## Glide Behaviour

`glide` is calculated at the original 64-sample rate, then its pitch increment
is applied per audio sample. Low source values move rapidly; high values move
more slowly, matching the original parameter curve. A held pitch has the same
final frequency as the fallback; only changes in pitch take longer to reach it.

## Audible Parameters

Normal performance always takes X and Y from the physical knobs or their CV
inputs; the browser's virtual XY position is editor-only. The stable Workshop
renderer uses saved `spread`, `glide`, `wobble_amount`, `wobble_speed`,
`saw_level`, `sub_level`, `comb_depth`, and `comb_mul` values. It deliberately
does not add the original pink-noise path, and keeps the original
motion-generating behavior disabled for stable knob operation. In the renderer,
`boc_amount` and `noise_level` are locked to zero. The web app still displays
and saves those two original fields, but changing them has no audible effect.

## Reset

The original web app can write its default preset values back to the editor.
Save that state with the same one-second Switch-Down hold to make those defaults
the card's new stored presets.

## Build

```sh
cmake -S . -B /private/tmp/workshop-buzzrito-glide -DCMAKE_BUILD_TYPE=Release
cmake --build /private/tmp/workshop-buzzrito-glide -j2
```

The generated test UF2 is intentionally not stored in Git. It is written to
the ignored `cards/buzzrito/test-artifacts/` directory when built for hardware
testing.
