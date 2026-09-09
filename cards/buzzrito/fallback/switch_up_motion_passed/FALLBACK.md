# Approved Switch Up Motion Fallback

This directory is a source snapshot of the hardware-passed main Buzzrito card.

- Clock: 192 MHz
- Includes: stable saw/sub renderer, comb, deterministic per-saw wobble,
  Switch Up motion recording, smooth interpolation, ping-pong for open paths,
  and CV1/CV2 axis overrides during playback
- Excludes: random XY wobble, pink noise, flash persistence, and Pulse2 motion
- Matching UF2: `../../uf2/previous-versions/switch_up_motion_passed/workshop_buzzrito_0.1.0_switch_up_motion_fallback.uf2`
- SHA-256: `307f52d9f73172937d5016bdadaafeefdb26c1d4a6712eddaa2c4f8c1c312ca9`

The simpler pre-noise and pre-wobble fallbacks remain available beside this
version. Restore this snapshot only when intentionally returning to the
hardware-passed Switch Up motion behavior.
