# Workshop Buzzrito

First-pass Workshop Computer port of the Buddies Buzzrito swarm oscillator.

## Controls

- Main: overall tune / pitch offset
- X: virtual XY pad X
- Y: virtual XY pad Y
- CV1: pitch modulation
- Pulse1: gate; unpatched means drone
- Audio Out 1/2: stereo Buzzrito output
- CV Out 1: approximate pitch CV monitor
- Pulse Out 1: gate monitor

## Notes

The original Buzzrito firmware runs the RP2040 at 200 MHz, 48 kHz audio, and
64-sample I2S blocks. This Workshop scaffold runs at 192 MHz and calls the
ported DSP once per sample from `ComputerCard::ProcessSample()`.

This is a buildable starting point, not yet a profiled final card. The next DSP
pass should measure `ProcessSample()` time and, if needed, refactor the
block-oriented engine into a dedicated single-sample renderer.

## Build

```sh
cmake -S cards/buzzrito -B cards/buzzrito/build -DCMAKE_BUILD_TYPE=Release
cmake --build cards/buzzrito/build -j2
```

The build expects `PICO_SDK_PATH` to point at
`/Users/adrianvos/coding/GitHub/pico-sdk`.
