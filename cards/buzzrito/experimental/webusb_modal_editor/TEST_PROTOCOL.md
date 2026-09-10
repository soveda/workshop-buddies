# Modal WebUSB Test Protocol

**Passed:** WebUSB app connection, normal-performance isolation, and preset
persistence across reboot.

1. Flash `uf2/workshop_buzzrito_webusb_modal_editor_persistence.uf2`.
2. Boot with switch Middle. Confirm normal sound, Switch-Up gesture recording,
   playback, and Pulse2 live takeover remain indistinguishable from the stable
   non-USB firmware. No USB device should appear.
3. Power down. Set the latched switch Up, connect USB, power on, and leave the
   switch Up for one second.
4. Open the original Buzzrito web app and connect. Confirm it can claim the
   `Workshop Buzzrito WebUSB` vendor device and read the seven presets.
5. Move one obvious setting in the web app and confirm the connection remains
   stable. Audio remains intentionally muted while editing.
6. Move the latched switch Down and hold it for one second. Confirm the card
   reboots into normal performance mode.
7. Confirm the edited setting affects the normal Buzzrito sound after reboot.
   Re-enter editor mode and check that the app reads back the same saved value.
8. Power-cycle with switch Middle and confirm normal performance behaviour and
   the saved presets return without stutter.
