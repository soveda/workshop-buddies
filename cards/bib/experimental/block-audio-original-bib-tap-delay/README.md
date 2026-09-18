# Workshop Block-Audio: Original Bib Tap-Delay Test

This separate test adds a Workshop interpretation of Bib's delay-mode spider
tap to the passed switch-actions build. It uses the original Bib 64-frame DSP
from the public [Buddies repository](https://github.com/soveda/buddies_public)
(GPL-3.0-or-later); the custom block transport does not use `ComputerCard.h`.
Workshop hardware-map portions derive from ComputerCard v0.3.0 by Chris
Johnson (MIT).

## Existing Controls

Main selects pages shown by LEDs 2-5: Drive/Delay Send, Delay Time/Feedback,
Reverb Send/Feedback, and Wet/Dry Mix/Output. Drive-page Switch Down toggles
wavefold. Mix-page Switch Down holds the dub gesture. LEDs 0 and 1 remain
transport-error indicators.

## New Tap Delay

In the Delay page (LED 3), Switch Down is a tap:

1. Press Down once to start a tap sequence.
2. Press Down again within one second to set delay time to the interval from
   the first press. Further presses within one second update it from that same
   first press, like Bib's original tap sequence.
3. After tapping, X is parked at the tapped delay time. Move X by roughly 2%
   of its travel to intentionally restore manual delay-time control.

The original pressure-sensitive spider can create multiple weighted delay taps.
The Workshop switch cannot measure pressure, so this version intentionally
uses a single, full-level tap. This preserves the useful tempo-setting
behaviour without inventing an unstable approximation.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_tap_delay_test.uf2`.

## Hardware Test Protocol

1. Flash the UF2, reset, and confirm the four Main/X/Y pages and both prior
   switch actions work exactly as in the passed switch-actions build.
2. Select LED 3. Set audible delay send and moderate feedback using LED 2 and
   LED 3 controls. Leave X still after entering the Delay page.
3. Press Switch Down twice about half a second apart. The repeats should move
   to approximately that half-second interval, with no click, glitch, or LED
   0/1 fault.
4. Repeat using clearly different intervals: a fast 100-200 ms setting, then
   a slower 700-900 ms setting. Confirm the delay follows each interval.
5. After a successful tap, do not move X: switch pages and return to LED 3.
   The tapped delay time should persist. Then move X clearly; it should take
   over delay-time control and sweep normally again.
6. Hold Down in LED 3, make repeated taps, and use the other controls while
   audio runs. There must be no repeated unintended triggers, unstable sound,
   white noise, freeze, or LED 0/1 indication.
7. Run a feedback-heavy tapped delay for 30 minutes. Report any distortion,
   pitch drift, lost controls, reset requirement, or transport error.
