# Workshop Block-Audio: Bib Guard, Wavefold, and Knob Pickup Test

This experiment combines the passed startup guard and wavefold cap with
per-page X/Y soft takeover. It includes original Bib 64-frame DSP from the
public [Buddies repository](https://github.com/soveda/buddies_public), under
GPL-3.0-or-later. Its custom Workshop transport does not use `ComputerCard.h`;
hardware-map portions derive from ComputerCard v0.3.0 by Chris Johnson (MIT).

## Included Behaviour

- Switch actions arm only after the switch settles in Middle or Up, preventing
  the reset-time phantom Down state from enabling wavefold.
- Wavefold is capped at Drive `3072`; regular soft-clip Drive remains full
  range.
- Main selects four retained X/Y pages on LEDs 2-5: Drive/Delay Send, Delay
  Time/Feedback, Reverb Send/Feedback, Wet/Dry Mix/Output.
- Switch Down still provides wavefold toggle, tap delay, and dub hold.

## Knob Pickup

When Main selects a page, its saved values remain unchanged. The current X/Y
positions are remembered but do not act immediately. Move either knob and its
parameter catches smoothly from the saved value toward the physical position.
Once the two positions meet, the parameter resumes direct tracking. This is
the original Bib software-knob catch-up approach scaled to Workshop controls.

LEDs 0 and 1 remain input-overrun and output-underrun indicators and must stay
off.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_startup_guard_wavefold_cap_knob_pickup_test.uf2`.

## Hardware Test Protocol

1. Reset at least ten times with Main in Drive mode and Switch Middle. Startup
   must be normal soft clipping, never wavefold fizz; LEDs 0/1 stay off.
2. In any page, create an obvious sound setting, park X/Y at very different
   positions, select another page, then return. The earlier setting must
   return unchanged, with no parameter jump.
3. Move X slowly after returning to a page. It should not instantly jump to
   the knob's parked position; instead it catches the saved parameter toward
   it. Repeat for Y in every page.
4. Check the bipolar centres of Delay Send and Wet/Dry Mix. Page changes and
   pickup movement must be click-free and must not create a feedback burst.
5. Toggle wavefold and sweep X to maximum. It should remain musical at the
   top; turn wavefold off and verify normal Drive returns.
6. Test tap delay and dub hold, then run active audio for 30 minutes with
   repeated page changes. Report any transport LED, noise, drift, freeze, or
   reset requirement.
