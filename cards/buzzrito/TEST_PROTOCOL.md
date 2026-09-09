# Workshop Buzzrito Test Protocol

## Artifact

- UF2: `cards/buzzrito/uf2/workshop_buzzrito_0.1.0_test.uf2`
- Clock: 192 MHz
- Audio rate: 48 kHz via `ComputerCard::ProcessSample()`, one stereo frame per callback
- Source mapping: Buddies Buzzrito preset map with Workshop Computer controls
- Current diagnostic behavior: simplified saw/sub swarm renderer, lower/narrower Main range, no saved motion, no random wobble, no noise, no comb

## Control Map

- Main: overall tune
- X: virtual XY pad X
- Y: virtual XY pad Y
- Audio/CV In 1: pitch modulation, roughly 3 mV per signed ADC count
- CV1: virtual XY pad X modulation
- CV2: virtual XY pad Y modulation
- Pulse1: gate input; unpatched means drone
- Audio Out 1/2: stereo output
- CV Out 1: pitch CV monitor
- Pulse Out 1: gate monitor
- LEDs 0/1: X position/activity
- LEDs 2/3: Y position/activity
- LED 4: gate level
- LED 5: combined X/Y activity

## Flash Test

1. Put the Workshop Computer into BOOTSEL/UF2 mode.
2. Copy `cards/buzzrito/uf2/workshop_buzzrito_0.1.0_test.uf2` to the mounted RP2040 drive.
3. Let the board reboot.
4. Patch Audio Out 1 and 2 to a mixer or scope at conservative gain.
5. Leave Pulse1 unpatched for the first pass; the firmware should drone.

Pass criteria:

- Board reboots after flashing.
- No boot loop or USB remount cycling.
- Audio Out 1/2 produce a continuous stereo tone or swarm texture.
- LEDs update when Main, X, and Y are moved.

## Control Sweep

1. Set Main near noon, X near noon, Y near noon.
2. Sweep Main slowly from minimum to maximum.
3. Sweep X from minimum to maximum while holding Main steady.
4. Sweep Y from minimum to maximum while holding Main steady.
5. Move X and Y together through the corners: low/low, high/low, low/high, high/high.

Expected behavior:

- Main changes overall pitch/tune across a broad range.
- The lowest Main settings should be clearly below the previous high-pitched build.
- X and Y reshape the Buzzrito swarm in clearly different ways.
- No hard lockups, sudden silence, or harsh digital clipping during knob travel.
- LED pairs should move in opposite brightness patterns as X and Y cross center.

## Gate And Pitch CV

1. Patch a slow gate or square LFO into Pulse1.
2. Confirm audio opens and closes with the gate.
3. Confirm Pulse Out 1 follows the gated state.
4. Patch a slow bipolar CV into Audio/CV In 1.
5. Sweep Main while Audio/CV In 1 is patched.

Expected behavior:

- Pulse1 gates the output with a short smoothing transition.
- With Pulse1 unplugged, the firmware returns to drone behavior.
- Audio/CV In 1 shifts pitch around the Main tune setting.
- CV Out 1 follows the approximate pitch control and stays within the Workshop Computer output range.

## X/Y CV Modulation

1. Set X and Y near noon.
2. Patch a slow bipolar CV or LFO into CV1.
3. Confirm CV1 moves the same sound region as the X knob.
4. Patch a different slow bipolar CV or LFO into CV2.
5. Confirm CV2 moves the same sound region as the Y knob.
6. Try X/Y knob sweeps while CV1/CV2 remain patched.

Expected behavior:

- CV1 offsets the virtual pad X position.
- CV2 offsets the virtual pad Y position.
- X/Y modulation remains bounded at the pad edges rather than wrapping or locking up.

## Audio Quality Checks

1. Listen at low, medium, and high Main settings.
2. Check both outputs independently on a scope or mixer.
3. Let the patch run for at least five minutes.
4. Move every control repeatedly while monitoring for dropouts.

Pass criteria:

- Both audio outputs are active.
- No repeated clicks caused by CPU underrun.
- No stuck DC output.
- No thermal or stability issue during a five-minute run.

## Notes To Capture

- Does Main's range feel too wide, too narrow, or reversed?
- Does this diagnostic build sound like a stable saw swarm instead of broadband noise?
- With X/Y knobs untouched and CV1/CV2 unpatched, does the sound stay stable instead of feeling like saved motion playback?
- Is the sub oscillator audible in at least one X/Y region?
- Are X and Y intuitive compared with the original XY pad?
- Does Audio/CV In 1 pitch scaling feel right?
- Do CV1 and CV2 feel like faithful X/Y pad modulation inputs?
- Does Pulse1 gating feel useful, or should it become a mode/chord trigger?
- After this UF2 passes, add switch behavior: short press taps the original Buzzrito bee/chord mode, press-and-hold manually opens/closes gate depending on Pulse1 patching.
- Are the LED behaviors helpful enough for a card without artwork?
- Any control ranges that should be curved, limited, or swapped before the next UF2.
