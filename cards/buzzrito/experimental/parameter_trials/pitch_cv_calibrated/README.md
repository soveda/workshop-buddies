# Buzzrito Pitch CV Calibration Trial

This isolated test derives from the hardware-passed full-depth staggered
wobble version. It changes only Audio/CV In 1 pitch scaling.

The Buzzrito renderer uses millivolts for pitch, but the Workshop Computer
provides signed ADC counts. The prior firmware added those counts directly,
making each input volt approximately 341 mV of pitch change. This trial uses
the hardware-proven C1ZZL3 calibration of 341 counts per volt, converting the
input to millivolts before it enters the original Buzzrito pitch path.

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

## Pitch Calibration

`Audio/CV In 1` now follows 1 V/oct using 341 input counts per volt, matching
the working C1ZZL3 setting. A Q12 fixed-point conversion is used so 341 counts
maps exactly to 1000 mV without a division in the audio callback.

The physical Main control continues to set the unpatched base pitch. A patched
Audio/CV In 1 adds calibrated pitch CV to that base. All passed full-depth
wobble, BOC, noise, motion, and WebUSB behavior is otherwise unchanged.

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
cmake -S . -B /private/tmp/workshop-buzzrito-pitch-cv -DCMAKE_BUILD_TYPE=Release
cmake --build /private/tmp/workshop-buzzrito-pitch-cv -j2
```

The generated test UF2 is intentionally not stored in Git. It is written to
the ignored `cards/buzzrito/test-artifacts/` directory when built for hardware
testing.
