# Workshop Buddies

Workshop Computer firmware cards adapted from, or inspired by, the open-source
[Buddies software](https://github.com/plinkysynth/buddies_public) by Plinky Synth.

## Source References

- Original software: [Buddies public repository](https://github.com/plinkysynth/buddies_public).
- Hardware and framework: [Workshop Computer repository](https://github.com/TomWhitwell/Workshop_Computer).

## Cards

- [Buzzrito / BuzzyBee](cards/buzzrito/README.md): hardware-tested stereo swarm
  oscillator, virtual XY controls, comb, wobble, note-memory chords, saved
  motion, and a separate WebUSB preset-editor mode.
- [Bib / Bibesque](cards/bib/README.md): hardware-tested 1.1.2-beta stereo
  delay/reverb using original 64-frame DSP and a custom DMA block-audio runtime.
  Catalogue status remains Beta.
- [Blueberry Smoothie (card 873)](cards/blueberry/README.md): hardware-tested 0.3.0 Beta
  quantised keyboard recorder with independent pitch/gate inputs and toggle recording.

## Porting Notes

The original Bib and Buzzrito firmware runs on RP2040 at 200 MHz with 48 kHz
audio and 64-sample blocks. These cards target the Workshop Computer at
192 MHz. Buzzrito uses a 48 kHz per-sample renderer; Bib retains block-based
processing. Blueberry uses ComputerCard for its control/sequencer runtime.
See each card README for build instructions, current UF2s, and archived versions.

The Buddies software is MIT licensed. Its hardware and graphic design have
separate licences and are not reused here. See each card's licence and
third-party notices for provenance. Blueberry is a new inspired implementation,
not a direct source port.

## Buzzrito Mapping

- Main: overall tune / pitch offset
- X: virtual XY pad X
- Y: virtual XY pad Y
- Audio/CV In 1: 1 V/oct pitch
- CV In 1 / 2: X / Y modulation, overriding the corresponding saved-motion axis
- Pulse In 1: gate; unpatched for drone
- Pulse In 2: return to live X/Y
- Switch Up: record motion; Middle: saved playback or live controls
- Switch Down: tap cycles chord mode; hold overrides the gate
- LEDs: four virtual pad corners, gate, and chord/recording feedback

Boot with Switch Up for the separate muted WebUSB editor mode. See the card
README for editing and saving presets.

## Bib Mapping

- Main: selects Drive, Delay, Reverb, or Mix page
- X / Y: page parameters, with pickup and CV In 1 / 2 modulation
- Pulse In 1: Bib-style clock quantisation of the preferred delay time
- Audio In / Out 1 and 2: independent stereo processing
- LEDs: page, effective X/Y values, and page-specific state

Page controls:

- Drive: X = drive, Y = bipolar delay send; Down toggles wavefold.
- Delay: X = time, Y = feedback; Down records multitap timing.
- Reverb: X = send, Y = feedback; Up accesses retained shimmer amount.
- Mix: X = wet/dry, Y = output level; Down holds dub feedback.
- Delay with Up: X = tape rate, Y = wow/flutter.

## Blueberry Smoothie Mapping

- CV In 1: quantised pitch from one Workshop 4 Voltages output.
- Pulse In 2: independent live gate from Slopes or an external source.
- CV Out 1 / Pulse Out 1: live or saved pitch/gate.
- Pulse In 1: playback clock; Pulse Out 2: loop restart pulse.
- Middle: free play, or continue an active recording after releasing Down.
- Down: press once to start a new take, press again to stop. LED 2 stays lit
  throughout recording, including after release to Middle.
- Up: play the loop, finishing an active take first.
- Main: cross 3 or 9 o'clock for one upward/downward transposition notch;
  return through centre to rearm.

Loops are held in RAM and clear on reset. No resting-voltage learning or user
calibration procedure is required. LED 0 reports input calibration only and can
remain off on a Computer with calibrated CV outputs.

The [0.3.0 test protocol](cards/blueberry/TEST_PROTOCOL.md) passed on Workshop
hardware on 2026-10-10. Previous passing 0.2.0 hold-to-record source and UF2
are preserved in [the archive](cards/blueberry/archive/0.2.0/).
