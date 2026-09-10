# Switch Up Motion Test Protocol

Start from the approved Buzzrito baseline sound: Main near noon, Pulse1 and
Pulse2 unpatched, and X/Y near centre.

1. Confirm the initial sound, X/Y map, LEDs 0-4, chord modes, pitch CV, and
   gate behavior match the approved baseline before using Switch Up.
2. Move Switch Up. LED 5 should become full brightness and the sound should
   continue to follow live X/Y controls.
3. Move X and Y slowly for roughly one second. Return the switch to Middle.
   The captured movement should loop with no knob jumps, rapid LED flicker,
   pitch instability, or broadband/wind-like sound.
4. While the loop plays, leave X/Y in different positions. The loop should
   continue to use the recorded path, matching the original Buzzrito behavior
   after a finger leaves its pad.
5. Move Switch Up again, leave X/Y stationary for at least 100 ms, then return
   to Middle. The new one-point path should remain stationary: this is the
   intended stop-motion gesture.
6. Patch CV1 while the loop plays. Saved X must be bypassed by the live X/CV1
   position while saved Y keeps looping. Repeat with CV2, then both inputs;
   both patched inputs must bypass the whole recorded path.
7. Hold Switch Up for more than two seconds. The first two seconds are kept;
   the firmware must remain stable and must not overwrite memory.
8. Use Switch Down short and held actions before, during, and after motion.
   Chord and gate behavior must remain unchanged. Pulse2 must have no effect
   on motion.

Fail immediately and return to the approved fallback UF2 if audio becomes
unstable, LED values flicker rapidly without a matching recorded movement,
controls appear to remap, or the baseline wobble/tone changes before any
recording is made.
