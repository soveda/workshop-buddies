# Stage 1 Test Protocol: USB MIDI Stability

1. Flash `uf2/workshop_buzzrito_usb_midi_stability.uf2`.
2. With no USB cable connected, verify the normal Buzzrito sound, knobs,
   switch-Up motion, and Pulse2 live takeover against `workshop_buzzrito_0.1.0.uf2`.
3. Connect USB-C to a computer. After roughly one second, confirm the host sees
   a MIDI device named `Workshop Buzzrito MIDI`.
4. Do not send MIDI notes, CCs, or SysEx. This stage intentionally ignores all
   received MIDI data.
5. Listen for at least five minutes with USB connected. Sweep Main, X, and Y;
   record and play an X/Y path; trigger Pulse2 during playback.
6. Disconnect and reconnect USB several times. Confirm no audio change, click,
   fizz, dropped oscillator, LED flicker, or boot/remount loop.

Pass only when the MIDI device enumerates and the audio is indistinguishable
from the promoted non-USB firmware before, during, and after USB connection.
