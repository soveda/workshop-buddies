# Workshop Block-Audio: Original Bib Shimmer Test

This separate test starts from the passed startup guard, wavefold cap, knob
pickup, and delay-feedback-cap build. It runs original Bib `process_bib()`
DSP over 64-frame blocks, using source from the public
[Buddies repository](https://github.com/soveda/buddies_public) under
GPL-3.0-or-later. The custom Workshop transport does not use `ComputerCard.h`;
hardware-map portions derive from ComputerCard v0.3.0 by Chris Johnson (MIT).

## Shimmer Layer

Main selects the Reverb page at LED 4. With the switch Middle, X is Reverb
Send and Y is Reverb Feedback as before. Set Switch Up to enter the retained
Shimmer layer:

- X changes shimmer amount using relative pickup, so entering Switch Up never
  applies its parked position as an unintended amount.
- Y is inactive in this layer and continues to preserve Reverb Feedback.
- Return Switch to Middle: X resumes Reverb Send and the chosen shimmer
  remains active.

Shimmer starts at zero. Its initial Workshop maximum is half of the original
touch-pressure range: the value is sent directly to `process_bib()` instead of
the original UI's `shimmer * 2` maximum. This gives a controlled first test of
the original shimmer reverb path.

All passed behaviour remains: startup switch guard, wavefold cap, per-page
pickup, tap delay, dub hold, and the `2490` delay-feedback ceiling. LEDs 0 and
1 remain transport-error indicators.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_shimmer_test.uf2`.

## Hardware Test Protocol

1. Flash and reset with Switch Middle. Confirm shimmer is absent at startup,
   and all pre-existing controls work normally with LEDs 0/1 off.
2. Select the Reverb page (LED 4), set a noticeable reverb send/feedback with
   Switch Middle, then move Switch Up. The sound must not jump merely from
   entering the layer.
3. Turn X gradually clockwise. Expect a controlled rising/octave-like shimmer
   component in the reverb tail. At maximum it should remain musical, with no
   harsh noise, persistent hum, clicking, or transport fault LED.
4. Return Switch Middle. Confirm X returns to Reverb Send, while the chosen
   shimmer remains audible. Re-enter Switch Up and confirm X picks up from the
   saved shimmer level rather than jumping.
5. Repeat with short and long delay settings, at the maximum safe delay
   feedback. Stop and unpatch input: every tail must decay to silence.
6. Run a shimmer-heavy patch for 30 minutes. Report any unstable pitch,
   noise, self-oscillation, freeze, reset requirement, or LED 0/1 event.
