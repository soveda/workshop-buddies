# Stage 1I Test Protocol: Parked Modal USB MIDI

1. Build this archived source locally and flash its generated UF2.
2. Normal performance boot: leave the switch in Middle, power the card, and
   verify sound, knobs, Switch-Up gesture record/playback, and Pulse2 live
   takeover against `workshop_buzzrito_0.1.0.uf2`. No MIDI device should appear.
3. USB editor boot: power down, move and keep the latched switch Up, connect
   USB, then power the card. After about one second, confirm the host sees a
   MIDI device named `Workshop Buzzrito MIDI`.
4. In USB editor mode, audio is intentionally muted and received MIDI is
   intentionally ignored. Disconnect/reconnect USB several times and confirm
   stable enumeration without resets.
5. Power cycle with the switch in Middle to leave editor mode. Confirm normal
   performance behaviour returns and Switch Up records gestures again.

Pass only when normal boot is indistinguishable from the promoted non-USB
firmware, and editor boot enumerates reliably without affecting the next normal
boot.
