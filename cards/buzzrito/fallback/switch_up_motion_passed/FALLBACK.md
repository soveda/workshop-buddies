# Approved Switch Up Motion Fallback

This directory is a source snapshot of the hardware-passed main Buzzrito card.

- Clock: 192 MHz
- Includes: stable saw/sub renderer, comb, deterministic per-saw wobble,
  Switch Up motion recording, smooth interpolation, ping-pong for open paths,
  and CV1/CV2 axis overrides during playback
- Excludes: random XY wobble, pink noise, flash persistence, and Pulse2 motion
- Firmware artifacts are deliberately not retained in Git; build this source
  snapshot locally when it is needed.

The simpler pre-noise and pre-wobble fallbacks remain available beside this
version. Restore this snapshot only when intentionally returning to the
hardware-passed Switch Up motion behavior.
