# Buzzrito Quarter-Depth Noise Test Protocol

The modal WebUSB, glide, BOC, and noise paths are inherited from passed
versions. This protocol tests only the increased `noise_level` cap.

## Test

**Result: passed.** Quarter-depth is the selected maximum noise range.

1. Flash `cards/buzzrito/test-artifacts/workshop_buzzrito_noise_quarter_depth_test.uf2`.
2. Boot with switch Middle. Confirm normal sound, Switch-Up gesture recording,
   playback, and Pulse2 live takeover remain indistinguishable from the stable
   non-USB firmware. No USB device should appear.
3. Power down. Set the latched switch Up, connect USB, power on, and leave the
   switch Up for one second.
4. Open the original Buzzrito web app and connect. Confirm it can claim the
   `Workshop Buzzrito WebUSB` vendor device and read the seven presets.
5. With a steady Main/X/Y position, set one preset's `noise_level` to zero,
   save with a one-second Switch-Down hold, and reboot to Middle. Confirm the
   held tone matches the passed full-depth BOC version.
6. Re-enter editor mode, set that same preset's `noise_level` to maximum,
   save, and reboot to Middle. With controls stationary, compare it to the
   passed one-sixteenth noise test: it should be four times more pronounced.
7. Check for no pitch movement, rapid windstorm modulation, clicking, fizz,
   stutter, unstable oscillator behavior, or change to the static X/Y sound
   position. Confirm full-depth BOC remains available.
8. Confirm Switch-Up gesture recording/playback, Pulse2 live takeover, gate,
   LEDs, and normal WebUSB editor entry still behave as in the passed fallback.
9. Re-enter editor mode and confirm the edited `noise_level` value remains visible.
