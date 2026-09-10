# Buzzrito WebUSB Editor Experiment

## Status

**Failed hardware experiment. Do not use this UF2 as a Buzzrito firmware.**

Two core assignments were tested:

- USB on core 0 / audio on core 1: the editor connected, but the audio became unstable.
- Audio on core 0 / USB on core 1: audio remained unstable and the editor did not connect.

The Workshop Computer audio runtime and the TinyUSB device initialization both
appear to require core 0. A future WebUSB implementation must therefore use a
core-0-compatible scheduling design; it must not modify the stable card until
that design has passed a no-browser audio test.

This is an isolated Workshop Buzzrito build for the published Buzzrito Presets
Editor. It starts from the hardware-passed switch-up-motion fallback and moves
only USB work to core 1; the 48 kHz ComputerCard audio renderer remains on its
hardware-passed core 0 arrangement.

## Compatibility

- USB VID: `0xcaff` (accepted by the published editor)
- USB PID: `0x2003`
- Product name: `Workshop Buzzrito WebUSB`
- One vendor-class (`0xFF`) interface with 64-byte bulk IN and OUT endpoints
- Original `0x0b47` preset, parameter, and XY packet format

The editor can read, edit, reset, import, and export all seven preset points.
The original parameters are transmitted intact. The current stable renderer
uses spread, saw/sub level, comb depth/pitch, and its limited deterministic
wobble mapping; glide, BOC, and noise remain compatibility fields and do not
add the original noise/motion engine.

## Control Handoff

Dragging the editor XY pad takes temporary control of the virtual pad. Moving
either physical X/Y control beyond a small threshold, or patching CV1/CV2,
returns immediately to the normal hardware-control and saved-motion behavior.
Preset edits remain in RAM for this experiment and are lost on power-off.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
```

The built image is `build/workshop_buzzrito_webusb_editor.uf2`.
