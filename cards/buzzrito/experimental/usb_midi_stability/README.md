# Buzzrito USB MIDI Stability Experiment

## Current Stage: 1 of 7 - First Variant Failed

The first hardware test enumerated as USB MIDI but changed oscillator behavior.
Its tight core-1 `tud_task()` loop is therefore not a valid foundation for this
card, even though the audio renderer itself is unchanged from the promoted
firmware. The next stage-1 variant must pace the MIDI task pump before any
MIDI-control work is introduced.

This experiment starts from the promoted Workshop Buzzrito 0.1.0 source and
adds only a class-compliant USB MIDI device. It does not read MIDI messages,
change presets, or communicate with the audio renderer.

Audio remains on core 0. Core 1 waits 100 ms, initializes TinyUSB with
`tud_init(0)`, and runs `tud_task()` continuously, following the proven
Cosmik C1zzl3 and Fr330hfr33 device pattern.

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

The output is `uf2/workshop_buzzrito_usb_midi_stability.uf2`.
