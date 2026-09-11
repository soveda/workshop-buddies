# Bib for Workshop Computer

**Status: first playable firmware implementation; not yet hardware tested.**

This is a Bib-inspired stereo delay and reverb card for the Music Thing
Workshop Computer. It translates Bib's touch-first interface into the
Workshop's three pots and momentary Z switch. It reuses the original project's
musical ideas, but not its artwork, panel graphics, or touch UI.

The card uses the bundled `ComputerCard.h` **v0.3.0** (12 May 2026).

## Controls

- Main: virtual slider; turn it through four regions to select a mode
- X/Y: the two controls for that mode
- Z down: tap, toggle, hold, or freeze according to the mode
- LEDs 0–1: binary mode page (`off/off`, `on/off`, `off/on`, `on/on`)
- LEDs 2–5: persistent level indicators for drive, delay feedback, reverb
  decay, and wet mix respectively

When you change Main page, X and Y use **pot pickup**. A control does not
change its newly selected parameter until the physical pot reaches that
parameter's saved position. This prevents abrupt sound changes when moving
between pages.

## Modes

- Mode 0 (Main fully counter-clockwise): X = drive; Y = delay send; Z tap
  toggles between overdrive and wavefold.
- Mode 1: X = delay time; Y = feedback; Z tap sets delay time from the
  interval between two taps.
- Mode 2: X = reverb send; Y = reverb decay; hold Z for a brighter,
  shimmer-like feedback colour. This is intentionally not a pitch shifter.
- Mode 3 (Main fully clockwise): X = wet/dry mix; Y = output level; hold Z
  to freeze the delay input for dub-style looping.

Audio In 1 is the primary input. With nothing patched to Audio In 2, the card
normalises Audio In 1 to both stereo channels. Audio Outs 1 and 2 carry the
stereo result.

## Build

With a configured Pico SDK:

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The resulting UF2 is `build/bib_workshop.uf2`.

## Implementation notes

The original Bib's 32k-sample stereo delay and reverb engine are designed for a
different block-audio hardware environment. This version uses a 16k-sample
stereo delay plus a small, fixed-point comb/allpass reverb. That fits safely
alongside a copy-to-RAM Workshop build and keeps every `ProcessSample()` call
small. The design is a starting point for listening tests, not a one-to-one
port of the original firmware.
