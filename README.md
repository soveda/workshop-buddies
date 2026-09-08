# Workshop Buddies

Workshop Computer firmware-card ports inspired by the open-source Buddies
firmware from `buddies_public`.

## Source References

- Original Buddies source: `/Users/adrianvos/coding/GitHub/buddies_public`
- Workshop Computer source and directive: `/Users/adrianvos/coding/GitHub/Workshop_Computer`
- Preferred Workshop target: `ComputerCard` firmware at 48 kHz

## Porting Notes

The original Bib and Buzzrito firmware runs on RP2040 at 200 MHz with 48 kHz
audio and 64-sample blocks. These ports will target the Workshop Computer at
192 MHz, using the Workshop Computer's standard controls, jacks, LEDs, and
firmware-card structure.

The Buddies software is MIT licensed. Logos, panel artwork, and graphic design
assets are not being reused here.

## Buzzrito Mapping

- Main: overall tune / pitch offset
- X: virtual XY pad X
- Y: virtual XY pad Y
- CV1: pitch input
- Pulse1: gate
- CV2: optional X/Y modulation or spread
- LEDs: XY, chord, preset, or gate feedback

Buzzrito is the clearest first port because its swarm oscillator DSP is already
fairly separate from the original touch hardware.

## Bib Mapping

- Main: virtual capacitive slider position / mode selection
- X: original A knob
- Y: original B knob
- Switch down: spider touch pad tap or hold
- LEDs: active mode and gesture feedback

Initial mode layout:

- Mode 0: X = drive, Y = delay send; switch-down toggles overdrive/wavefold
- Mode 1: X = delay time, Y = delay feedback; switch-down taps delay time
- Mode 2: X = reverb send, Y = reverb feedback/time; switch-down hold controls shimmer
- Mode 3: X = wet/dry mix, Y = output level; switch-down hold performs dub/freeze behavior

Bib is possible, but should be treated as a Bib-inspired Workshop delay/reverb
card rather than a one-to-one clone of the original hardware.
