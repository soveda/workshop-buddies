# Buzzrito USB MIDI Stability Experiment

## Current Stage: 1I of 7 - Parked Modal USB MIDI

Stages 1D and 1F enumerated while preserving some idle audio behaviour, but
both broke Switch-Up gesture recording, playback, and Pulse2 live takeover.
Stage 1E broke the oscillator immediately. Continuous USB during performance
is therefore rejected for this renderer.

This experiment uses the robust Workshop modal-USB pattern. On a normal boot,
TinyUSB is never initialized and core 1 parks in a hardware FIFO wait, leaving
the stable performance renderer without a busy second-core competitor.
Hold the latched switch **Up** while powering the card to enter USB MIDI editor
mode. The renderer then mutes before TinyUSB starts, so MIDI can enumerate
without competing with performance audio. It does not yet read MIDI messages,
change presets, or communicate with the audio renderer.

Editor mode remains muted until the next power cycle. A normal boot retains
Switch-Up as gesture record exactly as the promoted non-USB firmware does.

## Stages

1. **USB MIDI stability** - enumerate as `Workshop Buzzrito MIDI`; confirm
   audio remains identical with and without a USB host. Current stage.
2. **MIDI connection indication** - show harmless connection/activity without
   changing sound.
3. **Safe control handoff** - add a fixed core-1-to-core-0 mailbox, applied at
   the 1 kHz control cadence.
4. **SysEx editor protocol** - request/write Buzzrito preset data and virtual
   XY positions using 7-bit-safe USB MIDI SysEx.
5. **Web MIDI editor** - adapt the Buzzrito editor UI from WebUSB to Web MIDI.
6. **Controlled feature rollout** - validate saw/sub/comb edits, then XY
   takeover and physical-control takeover.
7. **Persistence** - add deferred flash saving only after live editing passes.

Do not modify the main Buzzrito firmware until this stage has passed hardware
testing.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
```

Generated firmware is intentionally not retained in Git.
