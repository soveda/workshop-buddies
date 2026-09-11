# Buzzrito Pitch CV Calibration Test Protocol

The passed full-depth wobble, BOC, noise, motion, and WebUSB paths are
inherited unchanged. This protocol tests only calibrated Audio/CV In 1 pitch.

## Test

1. Flash `cards/buzzrito/test-artifacts/workshop_buzzrito_pitch_cv_calibrated_test.uf2`.
2. Boot with switch Middle and set Main to a comfortable low-to-middle base
   pitch. Confirm the unpatched sound is unchanged from the passed version.
3. Patch a known accurate 1 V/oct source into Audio/CV In 1. Step it through
   0 V, +1 V, +2 V, and +3 V. Each one-volt increase should raise the held
   pitch by one octave, with no jitter or stepping while the CV is static.
4. Repeat with Main at a second position. The octave spacing should remain the
   same; Main merely changes the starting note.
5. At maximum X and maximum wobble, repeat the +1 V and +2 V steps. Confirm
   the passed oscillator stability is retained.
6. Confirm X/Y CV, Switch-Up gesture recording/playback, Pulse2 live takeover,
   gate, LEDs, and WebUSB editor entry still behave as in the passed version.
