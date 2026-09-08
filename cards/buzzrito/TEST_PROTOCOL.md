# Workshop Buzzrito Test Protocol

## Artifact

- UF2: `cards/buzzrito/uf2/workshop_buzzrito_0.1.0_test.uf2`
- Clock: 192 MHz
- Audio rate: 48 kHz via `ComputerCard::ProcessSample()`
- Source mapping: Buddies Buzzrito DSP with Workshop Computer controls

## Control Map

- Main: overall tune
- X: virtual XY pad X
- Y: virtual XY pad Y
- CV1: pitch modulation, roughly 3 mV per signed ADC count
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
- X and Y reshape the Buzzrito swarm in clearly different ways.
- No hard lockups, sudden silence, or harsh digital clipping during knob travel.
- LED pairs should move in opposite brightness patterns as X and Y cross center.

## Gate And CV

1. Patch a slow gate or square LFO into Pulse1.
2. Confirm audio opens and closes with the gate.
3. Confirm Pulse Out 1 follows the gated state.
4. Patch a slow bipolar CV into CV1.
5. Sweep Main while CV1 is patched.

Expected behavior:

- Pulse1 gates the output with a short smoothing transition.
- With Pulse1 unplugged, the firmware returns to drone behavior.
- CV1 shifts pitch around the Main tune setting.
- CV Out 1 follows the approximate pitch control and stays within the Workshop Computer output range.

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
- Are X and Y intuitive compared with the original XY pad?
- Does Pulse1 gating feel useful, or should it become a mode/chord trigger?
- Are the LED behaviors helpful enough for a card without artwork?
- Any control ranges that should be curved, limited, or swapped before the next UF2.
