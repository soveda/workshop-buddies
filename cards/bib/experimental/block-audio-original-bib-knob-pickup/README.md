# Workshop Block-Audio: Original Bib Knob-Pickup Test

This isolated test extends the passed original-Bib tap-delay build with
per-page X/Y soft takeover. The original 64-frame Bib DSP comes from the
public [Buddies repository](https://github.com/soveda/buddies_public), under
GPL-3.0-or-later. The custom Workshop transport does not use `ComputerCard.h`;
its hardware-map portions derive from ComputerCard v0.3.0 by Chris Johnson
(MIT).

## Controls

Main selects four pages shown by LEDs 2-5:

| LED | X | Y |
| --- | --- | --- |
| 2 | Drive | Delay send |
| 3 | Delay time | Delay feedback |
| 4 | Reverb send | Reverb feedback |
| 5 | Wet/dry mix | Output level |

Wavefold, dub hold, and tap delay retain their passed behaviours from the
previous version.

## New Soft Takeover

Each page retains its X/Y values when Main selects another page. On return,
the physical knobs do not immediately replace those values. Instead, moving a
knob smoothly catches the saved value toward its physical position. Once they
meet, it follows directly. This is a 12-bit adaptation of Bib's original
software-knob catch-up behaviour, not an abrupt mode change.

The special tap-delay pickup remains: after setting delay time by tap, move X
by about 2% to resume manual delay-time control. It then uses the same soft
takeover behaviour as every other control.

LEDs 0 and 1 remain transport-error indicators and must stay off.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_knob_pickup_test.uf2`.

## Hardware Test Protocol

1. Flash and reset. Confirm the existing page LEDs, wavefold toggle, dub hold,
   and tap delay work before exercising pickup.
2. In LED 2, set high Drive and a clearly audible delay send. Move X and Y to
   very different parked positions, then select LED 4. The drive/delay sound
   must not jump on the page change.
3. Set a distinctive reverb state in LED 4, park X/Y elsewhere, then return
   to LED 2. Its earlier drive/send state must return without a jump.
4. On each page, slowly move X and then Y from their parked positions. The
   selected parameter should begin moving smoothly rather than jumping to the
   knob's position. Continue moving until it reaches normal direct control.
5. Repeat around the bipolar centres of Delay Send and Wet/Dry Mix. Crossing
   the centre must be smooth and must not create a click, muted output, or
   sudden feedback burst.
6. Set delay time by tap, switch away and return, then move X. Confirm the
   tapped time persists until the deliberate pickup movement and manual X
   control resumes smoothly.
7. Run an active delay/reverb patch for 30 minutes while repeatedly changing
   pages and moving both controls. Report any LED 0/1, drift, noise, unstable
   sound, freeze, or reset requirement.
