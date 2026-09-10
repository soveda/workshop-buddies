# Buzzrito Glide Test Protocol

The modal WebUSB path is inherited from the passed fallback. This protocol
tests only the restored `glide` parameter.

1. Flash the locally generated `cards/buzzrito/test-artifacts/workshop_buzzrito_glide_test.uf2`.
2. Boot with switch Middle. Confirm normal sound, Switch-Up gesture recording,
   playback, and Pulse2 live takeover remain indistinguishable from the stable
   non-USB firmware. No USB device should appear.
3. Power down. Set the latched switch Up, connect USB, power on, and leave the
   switch Up for one second.
4. Open the original Buzzrito web app and connect. Confirm it can claim the
   `Workshop Buzzrito WebUSB` vendor device and read the seven presets.
5. Set one preset's `glide` to a low value, save with a one-second Switch-Down
   hold, and reboot to Middle. With a steady X/Y position, make repeated pitch
   steps using Main or Audio/CV In 1. The pitch should reach its final value
   rapidly and cleanly.
6. Re-enter editor mode and set that same preset's `glide` near its maximum.
   Save and reboot to Middle. Repeat the identical pitch steps. The pitch
   should now move noticeably more slowly, while reaching the same final note.
7. In both cases, check for continuous stable oscillator sound: no fizzy
   modulation, clicks during a held pitch, stutter, or change in the static
   X/Y sound position.
8. Confirm Switch-Up gesture recording/playback, Pulse2 live takeover, gate,
   LEDs, and normal WebUSB editor entry still behave as in the passed fallback.
9. Re-enter editor mode and confirm the edited `glide` value remains visible.
