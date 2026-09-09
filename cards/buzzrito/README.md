# Workshop Buzzrito

First-pass Workshop Computer port of the Buddies Buzzrito swarm oscillator.

## Controls

- Main: overall tune / pitch offset
- X: virtual XY pad X
- Y: virtual XY pad Y
- Audio/CV In 1: pitch modulation
- CV1: virtual XY pad X modulation
- CV2: virtual XY pad Y modulation
- Switch up: record a new virtual-pad path; return to Middle to play it
- Pulse1: gate; unpatched means drone
- Switch down short press: bee tap / chord mode cycle
- Switch down hold: opens the gate when Pulse1 is patched, closes the drone when Pulse1 is unpatched
- Audio Out 1/2: stereo Buzzrito output
- CV Out 1: approximate pitch CV monitor
- Pulse Out 1: gate monitor
- LEDs 0-3: virtual pad corners, ordered top-left, top-right, bottom-left, bottom-right
- LED 4: gate level
- LED 5: chord mode indicator, full brightness while recording

## Pad Axis Mapping

The original Buzzrito pad reports a two-dimensional position, not knob travel.
Workshop X/Y readings are converted into that pad coordinate space in
`main.cpp`. The square knob/CV range is tapered to the original pad's usable
edge geometry, so both controls continue to morph the sound near their ends.
The current hardware mapping uses normal X and inverted Y, so clockwise Y
matches the intended sound direction. LEDs retain the physical Y orientation,
so their top and bottom display is reversed relative to the virtual Y sound
coordinate.

## Current Test Behavior

The current UF2 is the hardware-passed Workshop Buzzrito port. It puts the
original Buzzrito base pitch near noon on Main and renders a bounded
per-sample saw/sub swarm with the original-style tuned comb section. At edge
presets it limits sub level relative to the saw, because direct knobs otherwise
make the original sub-only zones too broad.

Switch Up records a new X/Y path, replacing the previous one. Returning to
Middle plays the path: closed gestures loop and open gestures ping-pong. Hold
X/Y stationary while Up is selected, then return to Middle, to make a parked
one-point recording and stop movement. The path is about 2.05 seconds at full
length, sampled every 8 ms and interpolated/smoothed at 1 kHz. It is in RAM
only and is cleared by power cycling.

During playback, a patched CV1 bypasses recorded X while recorded Y continues;
CV2 does the converse. With both CV inputs patched, the whole recorded path is
bypassed by the live virtual-pad position. Pulse2 has no motion role.

The current wobble experiment is intentionally simpler than the original: a
very small, deterministic per-saw sine detune. It has no shared pitch drift or
XY movement. The original no-wobble build remains available as the fallback.

Pink noise is not rendered in the current test build. The bundled source
presets all specify zero `noise_level`, and the inactive noise path was
removed to retain the proven real-time execution behavior. This is a
deliberate porting decision: do not reintroduce pink noise without a separate
real-time execution-budget and audio-regression test.

`uf2/workshop_buzzrito_0.1.0_pre_noise_fallback.uf2` is the selected fallback:
the test-passed comb, micro-wobble, corrected-Y, and corrected-LED version.
The older no-wobble fallback is retained as
`uf2/workshop_buzzrito_0.1.0_pre_wobble_fallback.uf2`.

## Notes

The original Buzzrito firmware runs the RP2040 at 200 MHz, 48 kHz audio, and
64-sample I2S blocks. This Workshop scaffold runs at 192 MHz. The current
diagnostic build uses the original preset map but a simplified per-sample
oscillator renderer so Workshop Computer real-time behavior can be checked
before reintroducing the full Buzzrito comb/noise/wobble engine.

The audio renderer and motion control tick execute from RAM at 192 MHz. The
motion playhead is derived from the same 48 kHz audio interrupt, so it has no
independent control clock to drift against audio.

## Build

```sh
cmake -S cards/buzzrito -B cards/buzzrito/build -DCMAKE_BUILD_TYPE=Release
cmake --build cards/buzzrito/build -j2
```

The build expects `PICO_SDK_PATH` to point at
`/Users/adrianvos/coding/GitHub/pico-sdk`.
