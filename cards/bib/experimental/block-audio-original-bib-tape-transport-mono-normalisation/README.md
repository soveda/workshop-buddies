# Workshop Block-Audio: Bib Tape Transport Test

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

Shimmer starts at Bibesque's subtle `1024` setting. It is clamped to `3072`,
three quarters of the 12-bit control range, both for X and CV 1 modulation.
This preserves an audible startup character while leaving headroom below the
original maximum-pressure effect.

All passed behaviour remains: startup switch guard, wavefold cap, per-page
pickup, dub hold, and the `2490` delay-feedback ceiling.

## Delay Tape Transport

On the Delay page with Switch Up, X controls an independently picked tape
transport: fully counter-clockwise stops it, 12 o'clock is normal speed, and
fully clockwise is 2x. Y adds a slow 1.5 Hz wow/flutter around the selected
rate, with a maximum approximately plus or minus 25 percent. CV 1 and CV 2
modulate tape speed and wobble in this layer. Returning Switch Middle restores
ordinary Delay Time and Feedback, without altering their stored settings.

The direct rate entry is a small Workshop-only addition to the copied Bib DSP.
It enters immediately before Bib's existing tape-rate smoothing, while its
original delay read/write, feedback, and automatic long-delay slowdown remain
unchanged.

## Mono Input Normalisation

With Audio In 1 patched and Audio In 2 unpatched, Audio In 1 is copied to both
channels before the original stereo Bib DSP. A second patched input remains a
true independent right channel. With neither input patched, both channels are
silent and the normalisation probe cannot become audible noise.

## Delay Multitap

On the Delay page (Main selects LED 3), Switch Down records an equal-level
multitap phrase. The first press starts a one-second recording window. Each
following press inside that window adds a relative delay head, up to eight.
The newest press sets the overall delay period; earlier presses remain as
fractions of that period. This writes directly to original Bib's delay-tap
arrays, while retaining the conservative feedback ceiling.

Move X deliberately after recording to abandon the phrase, restore Bib's
single delay head, and return to ordinary X delay-time control. The original
spider pressure levels cannot be captured by the Workshop's on/off switch, so
every recorded head has equal level in this experiment.

## CV And Pulse 1

CV In 1 and CV In 2 now add bipolar modulation to X and Y respectively after
the physical-pot pickup. They therefore modulate the selected Main page
without making parked pots jump. On the Reverb Switch-Up shimmer layer, CV In
1 modulates shimmer amount. Recorded multitap phrases remain fixed; CV does
not retune their captured timing.

Pulse In 1 clock-quantises the active delay time to Bib-style 3/4, straight,
or dotted divisions across octaves. Valid clocks are 50 ms to 2 s. After a
clock is removed, the last clocked delay time stays in place until X is moved
deliberately, avoiding a jump back to the parked knob position. A manual tap
phrase takes priority over a clock.

## LEDs

This test restores Bibesque's control-oriented LED scheme. LEDs 0--1 show the
Main page as binary: off/off Drive, on/off Delay, off/on Reverb, on/on Mix.
LED 2 shows effective X and LED 3 effective Y, including CV, so their movement
directly verifies CV routing. LEDs 4--5 show useful state by page: wavefold
and ping-pong; clock lock and multitap head count; shimmer and mix; then dub
hold and output level.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_shimmer_multitap_cv_clock_test.uf2`.

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
7. Select the Delay page (LED 3). Press Switch Down once, then press it two to
   four more times at a steady rhythm, all within one second of the first.
   Confirm each press adds a repeat position and the last press sets the full
   delay time. At most eight presses after the initial press may add heads.
8. Keep the recorded phrase running at moderate feedback, then move X clearly
   in either direction. Confirm the multitap pattern clears, a single repeat
   remains, and X resumes manual delay-time control without clicks, hum, or a
   transport-error LED.
9. On each Main page, patch a slow bipolar CV into CV In 1, then CV In 2.
   Confirm LEDs 2 and 3 respectively brighten/dim with the effective X/Y
   values and the matching parameter responds, while the unpatched physical
   pot does not jump. On Reverb with Switch Up, confirm LED 2 and shimmer
   respond to CV In 1.
10. On the Delay page, patch a regular Pulse In 1 clock between 50 ms and
    2 s. Sweep X and confirm delay time snaps to nearby 3/4, straight, or
    dotted divisions over multiple octaves. Unpatch the clock: the last
    division must hold. Move X deliberately: normal manual delay time must
    resume without an audible jump, click, or transport-error LED.
11. On the Delay page, move Switch Up. Confirm the sound does not jump when
    entering the tape layer. Move X gradually from CCW through 12 o'clock to
    CW: expect stopped tape, normal speed, then 2x speed. Move Y upward for a
    slow, musical wow/flutter. Return Switch Middle and confirm X/Y resume
    ordinary delay time/feedback with no click, sustained hum, or fault.
