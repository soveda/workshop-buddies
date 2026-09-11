# Buzzrito Original Wobble Test Protocol

The modal WebUSB, glide, BOC, and noise paths are inherited from passed
versions. This protocol tests only the original per-saw `wobble_amount` path.

## Test

1. Flash `cards/buzzrito/test-artifacts/workshop_buzzrito_original_wobble_quarter_test.uf2`.
2. Boot with switch Middle. Confirm normal sound, Switch-Up gesture recording,
   playback, and Pulse2 live takeover remain indistinguishable from the stable
   non-USB firmware. No USB device should appear.
3. Power down. Set the latched switch Up, connect USB, power on, and leave the
   switch Up for one second.
4. Open the original Buzzrito web app and connect. Confirm it can claim the
   `Workshop Buzzrito WebUSB` vendor device and read the seven presets.
5. With a steady Main/X/Y position, set one preset's `wobble_amount` to zero,
   save with a one-second Switch-Down hold, and reboot to Middle. Confirm the
   held tone matches the passed full-depth BOC and quarter-noise version.
6. Leave `wobble_speed` at the preset value. Re-enter editor mode, set that
   preset's `wobble_amount` to maximum, save, and reboot to Middle. With
   controls stationary, listen for restrained independent saw movement.
7. Check for no rapid windstorm modulation, clicking, fizz, stutter, unstable
   oscillator behavior, or change to the static X/Y sound position. Confirm
   full-depth BOC and quarter-depth noise remain available.
8. Confirm Switch-Up gesture recording/playback, Pulse2 live takeover, gate,
   LEDs, and normal WebUSB editor entry still behave as in the passed fallback.
9. Re-enter editor mode and confirm the edited wobble values remain visible.
