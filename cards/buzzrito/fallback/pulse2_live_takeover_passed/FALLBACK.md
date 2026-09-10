# Pulse2 Live Takeover Fallback

This is the hardware-passed standard Buzzrito build with Pulse2 rising-edge
live X/Y takeover. It preserves the prior stable motion renderer and has no
WebUSB runtime.

- Pulse2 rising edge stops saved-motion playback and restores live X/Y.
- The saved path remains in RAM until replaced by a new Switch-Up recording or
  removed by power cycling.
- Matching UF2: `../../uf2/previous-versions/pulse2_live_takeover_passed/workshop_buzzrito_0.1.0_pulse2_live_takeover_fallback.uf2`
- SHA-256: `e660db2c2b498667f6bddb1faad5a68dbc266eb9008747cd4e2dd157b75e4358`
