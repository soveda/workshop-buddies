# Buzzrito Noise Test Protocol

The modal WebUSB, glide, and BOC paths are inherited from passed versions. This
protocol tests only the restored, depth-limited `noise_level` parameter.

## Test

1. Flash the locally generated `cards/buzzrito/test-artifacts/workshop_buzzrito_noise_test.uf2`.
2. Boot with switch Middle. Confirm normal sound, Switch-Up gesture recording,
   playback, and Pulse2 live takeover remain indistinguishable from the stable
   non-USB firmware. No USB device should appear.
3. Power down. Set the latched switch Up, connect USB, power on, and leave the
   switch Up for one second.
4. Open the original Buzzrito web app and connect. Confirm it can claim the
   `Workshop Buzzrito WebUSB` vendor device and read the seven presets.
5. With a steady Main/X/Y position, set one preset's `noise_level` to zero,
   save with a one-second Switch-Down hold, and reboot to Middle. Confirm the
   held tone matches the passed BOC version.
6. Re-enter editor mode, set that same preset's `noise_level` to maximum,
   save, and reboot to Middle. With controls stationary, listen for a subtle
   stereo pink-noise texture.
7. Return `noise_level` to zero and confirm the texture disappears completely.
   Check for no rapid windstorm modulation, pitch change, clicking, fizz,
   stutter, or change to the static X/Y sound position at either setting.
8. Confirm Switch-Up gesture recording/playback, Pulse2 live takeover, gate,
   LEDs, and normal WebUSB editor entry still behave as in the passed fallback.
9. Re-enter editor mode and confirm the edited `noise_level` value remains visible.
