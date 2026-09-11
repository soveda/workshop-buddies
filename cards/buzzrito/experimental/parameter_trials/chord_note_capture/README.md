# Buzzrito Original Chord Note-Capture Trial

This isolated test derives from the calibrated pitch-CV version. It replaces
the Workshop-only fixed interval chord table with the original Buzzrito
note-memory behavior.

The original chord modes are not fixed chord qualities. Modes 1-4 set the
number of stable input pitches that can be retained. The saws then cycle
through those retained notes. This implementation has passed hardware testing
and was judged musical across the chord modes.

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

## Chord Note Capture

Set the desired chord mode with short Switch-Down presses. With mode 2, 3, or
4 selected, set a first pitch and let it settle briefly, move Main or pitch CV
to a second pitch and let it settle, then repeat for further notes. The card
retains the most recent 2, 3, or 4 notes and distributes the saws across them.
Mode 1 immediately returns to a single live pitch.

The detector matches the original: it samples pitch once per 64 audio samples,
requires a 16-sample history stable within 30 mV, and accepts a new note only
when it differs by more than 30 mV. The passed calibrated 1 V/oct path and all
other behavior are unchanged.

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
cmake -S . -B /private/tmp/workshop-buzzrito-chord-capture -DCMAKE_BUILD_TYPE=Release
cmake --build /private/tmp/workshop-buzzrito-chord-capture -j2
```

The generated test UF2 is intentionally not stored in Git. It is written to
the ignored `cards/buzzrito/test-artifacts/` directory when built for hardware
testing.
