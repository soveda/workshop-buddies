# Pulse2 Live Takeover Fallback

This is the hardware-passed standard Buzzrito build with Pulse2 rising-edge
live X/Y takeover. It preserves the prior stable motion renderer and has no
WebUSB runtime.

- Pulse2 rising edge stops saved-motion playback and restores live X/Y.
- The saved path remains in RAM until replaced by a new Switch-Up recording or
  removed by power cycling.
- Firmware artifacts are deliberately not retained in Git; build this source
  snapshot locally when it is needed.
