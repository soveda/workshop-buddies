# Buzzrito Switch Up Motion Experiment

An isolated trial based on the approved pre-noise baseline. The baseline card,
its fallback source, and its approved UF2 are not changed by this experiment.

## Motion Interaction

- Switch Up starts a fresh X/Y recording and replaces the previous path.
- While Up is selected, audio follows the live X/Y knobs and CV1/CV2.
- Returning to Middle plays the captured path as a loop.
- Holding Up with X/Y stationary, then returning to Middle, creates a
  one-point loop. This is the Workshop equivalent of holding one place on the
  original Buzzrito touch pad and is the intended way to stop movement.
- Pulse2 has no motion role.
- LED 5 is full brightness while recording; in Middle it returns to chord-mode
  indication.
- A patched CV1 overrides saved X while saved Y continues to play; a patched
  CV2 does the converse. With both patched, both recorded axes are bypassed.

The path holds at most 256 points, sampled every 8 ms, for about two seconds
of motion. Playback linearly interpolates and smooths those points at 1 kHz to
avoid audible coordinate stepping. Closed gestures loop; open gestures
ping-pong, avoiding an end-to-start jump as on the original. It deliberately
has no flash saving, dynamic allocation, buffer resizing, multicore activity,
or pink-noise path.

## Real-Time Design

Knob/CV/switch reads remain in `ProcessSample()` as required by the Workshop
Computer API. Recorder state and buffer access occur only every 48 samples
(1 kHz); path points are captured at 125 Hz and interpolated at 1 kHz. The
existing stable saw/sub, comb, and deterministic wobble renderer is otherwise
unchanged.

## Build

```sh
cd cards/buzzrito/experimental/switch_up_motion
cmake -B build
cmake --build build -j2
```

The output is `build/workshop_buzzrito_switch_up_motion.uf2`.
