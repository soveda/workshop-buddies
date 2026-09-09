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
- Switch down short press: bee tap / chord mode cycle
- Switch down hold: opens the gate when Pulse1 is patched, closes the drone when Pulse1 is unpatched
- Audio Out 1/2: stereo Buzzrito output
- CV Out 1: approximate pitch CV monitor
- Pulse Out 1: gate monitor
- LED 5: chord mode indicator

## Pad Axis Mapping

The original Buzzrito pad reports a two-dimensional position, not knob travel.
Workshop X/Y readings are converted into that pad coordinate space in
`main.cpp` with `kInvertXKnob` and `kInvertYKnob` flags available if either
physical knob feels backwards during testing.

## Current Test Behavior

The current test UF2 is a stability diagnostic. It holds a stable virtual pad
position from X/Y knobs plus patched CV1/CV2, puts the original Buzzrito base
pitch near noon on Main, and renders a bounded per-sample saw/sub swarm. It deliberately bypasses
the original saved motion, random wobble, noise, and comb sections while we
verify that pitch, pad position, and the sub oscillator can stay parked.

## Notes

The original Buzzrito firmware runs the RP2040 at 200 MHz, 48 kHz audio, and
64-sample I2S blocks. This Workshop scaffold runs at 192 MHz. The current
diagnostic build uses the original preset map but a simplified per-sample
oscillator renderer so Workshop Computer real-time behavior can be checked
before reintroducing the full Buzzrito comb/noise/wobble engine.

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
