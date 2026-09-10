# Approved Pre-Noise Baseline

This directory is a source snapshot of the hardware-approved Buzzrito baseline.

- Clock: 192 MHz
- Audio behavior: stable saw/sub renderer, original-style comb, subtle deterministic per-saw wobble
- Excludes: saved motion, random XY wobble, pink noise, and Switch Up recording
- Firmware artifacts are deliberately not retained in Git; build this source
  snapshot locally when it is needed.

Restore these source files to the card root only when intentionally returning to
this known-good baseline.
