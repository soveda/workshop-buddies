# Buzzrito Original Chord Note-Capture Test Protocol

The passed full-depth wobble, BOC, noise, motion, WebUSB, and calibrated pitch
paths are inherited unchanged. This protocol tests original chord note capture.

## Result

Passed: the original note-memory chord behavior sounds musical in hardware.

## Test

1. Flash `cards/buzzrito/test-artifacts/workshop_buzzrito_chord_note_capture_test.uf2`.
2. Boot with switch Middle and make short Switch-Down presses until mode 1 is
   selected. Confirm a single live pitch follows Main or Audio/CV In 1.
3. Select mode 2. Hold one pitch for roughly 25 ms, move to a musical second
   pitch such as a fifth or octave, and hold it for roughly 25 ms. Confirm the
   sound contains those two captured notes, rather than a fixed interval table.
4. Select mode 3 and teach a major or minor triad one stable pitch at a time.
   Confirm the three retained notes remain musical after Main or pitch CV stops.
5. Select mode 4 and teach a four-note chord. Confirm a fifth input pitch
   replaces the oldest retained note.
6. Return to mode 1. Confirm it immediately returns to one live pitch.
7. Confirm calibrated 1 V/oct pitch, X/Y CV, Switch-Up motion, Pulse2 live
   takeover, gate, LEDs, and WebUSB editor mode continue to behave normally.
