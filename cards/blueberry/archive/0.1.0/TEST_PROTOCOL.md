# Workshop Blueberry First-Pass Test Protocol

## Setup

1. Patch one **4 Voltages** output to **Audio In 1**.
2. Patch **CV Out 1** to a 1 V/oct oscillator and **Pulse Out 1** to its gate
   or an envelope generator.
3. Reset with all 4 Voltages buttons released and the Workshop switch in
   Middle. Wait one second.

## Free Play

1. Press each 4 Voltages button and combinations of buttons.
2. Expect a stable quantised pentatonic pitch and a high gate while the input
   differs from its learned resting voltage.
3. Release all buttons. Expect the gate to close.
4. Turn Main past 3 o'clock, return to centre, then play again. Expect a
   Blueberry-style upward fifth step. Repeat: the next step reaches an octave.
5. Turn Main past 9 o'clock after returning to centre. Expect the reverse
   sequence.

## Recording And Free Loop Playback

1. Hold Switch Down while playing a short 4 Voltages phrase, then release.
2. LED 2 should be lit only during the held recording.
3. Move Switch Up. Expect the phrase to repeat with the same timing and
   pitch/gate pattern. Pulse Out 2 marks each loop restart.
4. Return to Middle. Expect live free play again; the loop remains in RAM.

## Clocked Playback

1. Patch a steady clock to Pulse In 1.
2. With Switch Up, expect each rising clock edge to advance to the next
   captured note event. LED 3 indicates that the clock input is patched.
3. Remove the clock. Expect playback to resume the phrase's recorded timing.

## Calibration Indicator

1. After startup, LED 0 on means calibrated Audio In conversion was found in
   EEPROM. Off means the card is using ComputerCard's approximate conversion.
2. Both states should remain playable; calibrated input is expected to give
   more repeatable pitch placement between Workshop Computers.
