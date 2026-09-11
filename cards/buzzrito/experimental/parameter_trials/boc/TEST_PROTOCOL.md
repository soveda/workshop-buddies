# Buzzrito BOC Test Protocol

The modal WebUSB and glide paths are inherited from passed versions. This
protocol tests only the restored, depth-limited `boc_amount` parameter.

## Test

**Result: passed.**

1. Flash the locally generated `cards/buzzrito/test-artifacts/workshop_buzzrito_boc_test.uf2`.
2. Boot with switch Middle. Confirm normal sound, Switch-Up gesture recording,
   playback, and Pulse2 live takeover remain indistinguishable from the stable
   non-USB firmware. No USB device should appear.
3. Power down. Set the latched switch Up, connect USB, power on, and leave the
   switch Up for one second.
4. Open the original Buzzrito web app and connect. Confirm it can claim the
   `Workshop Buzzrito WebUSB` vendor device and read the seven presets.
5. With a steady Main/X/Y position, set one preset's `boc_amount` to zero,
   save with a one-second Switch-Down hold, and reboot to Middle. Confirm the
   held tone matches the passed glide version.
6. Re-enter editor mode, set that same preset's `boc_amount` to maximum, save,
   and reboot to Middle. With the controls stationary, listen for restrained,
   slow shared pitch movement.
7. Confirm there is no rapid windstorm modulation, clicking, fizz, stutter,
   unstable base pitch, or change to the static X/Y sound position.
8. Confirm Switch-Up gesture recording/playback, Pulse2 live takeover, gate,
   LEDs, and normal WebUSB editor entry still behave as in the passed fallback.
9. Re-enter editor mode and confirm the edited `glide` value remains visible.
