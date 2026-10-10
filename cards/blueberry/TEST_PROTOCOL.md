# Workshop Blueberry 0.2.0 Test Protocol

Result: all checks reported passing on the Workshop System on 2026-10-10.
The tested firmware is `uf2/workshop_blueberry_0.2.0.uf2`; no firmware changes
were made after this pass report.

## Setup

1. Patch one **4 Voltages** output to **CV In 1**.
2. Patch **CV Out 1** to a 1 V/oct oscillator and **Pulse Out 1** to its gate
   or an envelope generator.
3. Patch bottom Slopes output to **Pulse In 2**, with Loop/Falling selected,
   or use an external gate/trigger. Reset Computer with Switch Middle.
   No special 4 Voltages selection is needed.

For Workshop-only audio, patch CV Out 1 to the top oscillator pitch input,
its sine to Ring Mod upper input, and Pulse Out 1 to top Slopes input.
Set top Slopes to Falling with Loop off; patch its output to Ring Mod lower
input, then Ring Mod output to Mix. Turn oscillator FM down.

## Free Play

1. Press each 4 Voltages button and combinations of buttons.
2. Expect quantised pentatonic pitch. Pulse Out 1 and LED 4 follow the separate
   Pulse In 2 gate; latched pitch selection does not hold the gate on.
3. Unpatch Pulse In 2. After jack detection settles, expect gate off regardless
   of pitch/button changes. Repatch it to resume gates without resetting.
4. Turn Main past 3 o'clock, return to centre, then play again. Expect a
   Blueberry-style upward fifth step. Repeat: the next step reaches an octave.
5. Turn Main past 9 o'clock after returning to centre. Expect the reverse
   sequence.

## Recording And Free Loop Playback

1. Hold Switch Down while selecting pitches and feeding gates into Pulse In 2,
   then release. Leave Pulse In 1 unpatched for this test.
2. LED 2 should be lit only during the held recording.
3. Move Switch Up. Expect the phrase to repeat with the same timing and
   pitch/gate pattern. Pulse Out 2 marks each loop restart.
4. Return to Middle. Expect live free play again; the loop remains in RAM.
5. In Up, change the live pitch and gate or unpatch Pulse In 2. The saved
   phrase should remain unchanged. In Middle those inputs should control
   the output again. Record once with Pulse In 2 unpatched: that loop should
   have no output gates.

## Clocked Playback

1. Patch a steady clock to Pulse In 1.
   With only the Workshop System, move bottom Slopes from Pulse In 2 to Pulse
   In 1 after recording. The saved gates no longer need a live gate source.
2. With Switch Up, expect each rising clock edge to advance to the next
   captured note event. LED 3 indicates that the clock input is patched.
3. Remove the clock. Expect playback to resume the phrase's recorded timing.
4. Captured events include gate-off states, so not every clock edge will sound
   a note. Try different clock rates and confirm most edges are not missed.

## Calibration Indicator

1. After startup, LED 0 on means input calibration coefficients were found in
   EEPROM. Off means the card is using ComputerCard's approximate conversion.
2. Both states should remain playable; calibrated input is expected to give
   more repeatable pitch placement between Workshop Computers. CV inputs are
   less precise than Audio inputs; check stable pitch at fixed voltages.
3. Leave a loop running for 15 minutes, then check recording and free play.
   Reset clears the loop. Timing is recorded at 1 ms resolution; sampled
   short gate triggers are retained for at least one tick.
