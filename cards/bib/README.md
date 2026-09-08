# Bib Staging Notes

Bib is staged as the second Workshop Buddies port after Buzzrito.

## Proposed Controls

- Main: virtual capacitive slider position / mode selection
- X: original A knob
- Y: original B knob
- Switch down: spider touch pad tap or hold
- LEDs: active mode and gesture feedback

## Proposed Modes

- Mode 0: X = drive, Y = delay send; switch-down toggles overdrive/wavefold
- Mode 1: X = delay time, Y = delay feedback; switch-down taps delay time
- Mode 2: X = reverb send, Y = reverb feedback/time; switch-down hold controls shimmer
- Mode 3: X = wet/dry mix, Y = output level; switch-down hold performs dub/freeze behavior

## Implementation Notes

Treat this as a Bib-inspired Workshop delay/reverb card. Reuse the MIT-licensed
software DSP where it makes sense, but do not reuse Buddies artwork or panel
graphics. The large delay and reverb buffers need a RAM budget check before the
card is copied to RAM.

