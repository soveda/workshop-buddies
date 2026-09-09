# Workshop Buzzrito

First-pass Workshop Computer port of the Buddies Buzzrito swarm oscillator.

## Controls

- Main: overall tune / pitch offset
- X: virtual XY pad X
- Y: virtual XY pad Y
- Audio/CV In 1: pitch modulation
- CV1: virtual XY pad X modulation
- CV2: virtual XY pad Y modulation
- Pulse1: gate; unpatched means drone
- Audio Out 1/2: stereo Buzzrito output
- CV Out 1: approximate pitch CV monitor
- Pulse Out 1: gate monitor

## Current Test Behavior

The current test UF2 holds a stable virtual pad position from X/Y knobs plus
patched CV1/CV2. It disables Buzzrito's internal random pitch/saw wobble, uses
a lower Main pitch range, and renders one sample per audio callback to avoid
periodic block-render timing spikes.

## Staged Next Change

After the current test UF2 passes, add switch behavior:

- Short press: equivalent to tapping the bee on the original Buzzrito, for chord mode changes.
- Press and hold: manual gate open/close behavior, coordinated with Pulse1 patch detection.

## Notes

The original Buzzrito firmware runs the RP2040 at 200 MHz, 48 kHz audio, and
64-sample I2S blocks. This Workshop scaffold runs at 192 MHz and keeps the
original 64-sample shift constants for glide/control smoothing while rendering
one stereo sample per `ComputerCard::ProcessSample()` call.

This is a buildable starting point, not yet a profiled final card. The next DSP
pass should measure the block render time and confirm it leaves enough headroom
inside the Workshop Computer audio callback.

## Build

```sh
cmake -S cards/buzzrito -B cards/buzzrito/build -DCMAKE_BUILD_TYPE=Release
cmake --build cards/buzzrito/build -j2
```

The build expects `PICO_SDK_PATH` to point at
`/Users/adrianvos/coding/GitHub/pico-sdk`.
