# Workshop Buzzrito Test Protocol

## Artifact

- UF2: `cards/buzzrito/uf2/workshop_buzzrito_0.1.0_test.uf2`
- Clock: 192 MHz
- Audio rate: 48 kHz via `ComputerCard::ProcessSample()`, one stereo frame per callback
- Source mapping: Buddies Buzzrito preset map with Workshop Computer controls
- Current behavior: hardware-passed saw/sub swarm renderer with moderated edge sub levels, tuned comb, deterministic per-saw wobble, Switch Up motion recording, and no pink-noise path.

## Control Map

- Main: overall tune
- X: virtual XY pad X
- Y: virtual XY pad Y
- Audio/CV In 1: pitch modulation, roughly 3 mV per signed ADC count
- CV1: virtual XY pad X modulation
- CV2: virtual XY pad Y modulation
- Switch up: record a new X/Y path; return to Middle to play it
- Switch down short press: bee tap / chord mode cycle
- Switch down hold: opens the gate when Pulse1 is patched, closes the drone when Pulse1 is unpatched
- Pulse1: gate input; unpatched means drone
- Audio Out 1/2: stereo output
- CV Out 1: pitch CV monitor
- Pulse Out 1: gate monitor
- LEDs 0-3: virtual pad corners, ordered top-left, top-right, bottom-left, bottom-right
- LED 4: gate level
- LED 5: chord mode indicator; full brightness while recording

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
6. Confirm that clockwise X agrees with the corner LEDs. Y uses the intended sound direction while the physical LED top/bottom display is reversed from the virtual Y coordinate.

Expected behavior:

- Main changes overall pitch/tune across a broad range.
- The original Buzzrito base note should sit near noon on Main, with audible range below it at lower settings.
- X and Y reshape the Buzzrito swarm in clearly different ways, including past roughly the 2-3 o'clock region.
- Extreme X/Y positions may become sub-forward, but should retain a clear saw layer and respond to the other control through both oscillator balance and comb resonance.
- Wobble should be limited to a slow, subtle widening of the saw swarm. It must not move the overall pitch, create random XY-style changes, or sound like wind.
- Pink noise is intentionally absent; bundled source presets request zero `noise_level` and the inactive renderer path was removed.
- Do not reintroduce pink noise without a separate real-time execution-budget and audio-regression test: executing its inactive generator path changed the audible result on hardware.
- No hard lockups, sudden silence, or harsh digital clipping during knob travel.
- LEDs 0-3 should crossfade toward the corresponding virtual-pad corner as X/Y move.

## Gate And Pitch CV

1. Patch a slow gate or square LFO into Pulse1.
2. Confirm audio opens and closes with the gate.
3. Confirm Pulse Out 1 follows the gated state.
4. Hold the switch down while Pulse1 is patched low.
5. Unpatch Pulse1 and hold the switch down.
6. Patch a slow bipolar CV into Audio/CV In 1.
7. Sweep Main while Audio/CV In 1 is patched.

Expected behavior:

- Pulse1 gates the output with a short smoothing transition.
- With Pulse1 unplugged, the firmware returns to drone behavior.
- Holding the switch opens the gate when Pulse1 is patched.
- Holding the switch closes/mutes the drone when Pulse1 is unpatched.
- Audio/CV In 1 shifts pitch around the Main tune setting.
- CV Out 1 follows the approximate pitch control and stays within the Workshop Computer output range.

## Chord Mode

1. Leave Pulse1 unpatched.
2. Short-press the switch down and release.
3. Repeat four times.

Expected behavior:

- Each short press cycles the bee/chord mode: 1, 2, 3, 4, then back to 1.
- LED 5 steps through four brightness levels.
- Chord modes 2-4 add interval stacks across the saw bank.

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

## Saved Motion

1. Leave CV1 and CV2 unpatched. Move Switch Up, sweep X/Y for up to two seconds, then return to Middle.
2. Confirm a closed path loops and an open path moves forward then backward without a sharp end-to-start jump.
3. Record a stationary point by holding X/Y still in Up for at least 100 ms, then returning to Middle.
4. During playback, patch CV1 and confirm live X/CV1 bypasses only saved X while saved Y continues.
5. Repeat with CV2, then patch both inputs and confirm the complete saved path is bypassed.
6. Confirm Pulse2 has no effect on motion.

Expected behavior:

- Switch Up replaces the prior path; it does not append to it.
- A stationary recording provides the stop-motion gesture.
- The full-length recording is about 2.05 seconds, retained only in RAM until power off.
- Motion playback remains smooth, stable, and phase-locked to the audio callback.

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
- Does Switch Up capture and replay the intended X/Y gesture without clicks, stutter, or unintended control changes?
- Is the sub oscillator audible in at least one X/Y region?
- Do saw/sub levels across the X/Y map feel closer to the original Buzzrito module?
- Are X and Y intuitive compared with the original XY pad?
- Should `kInvertXKnob` or `kInvertYKnob` be flipped?
- Does Audio/CV In 1 pitch scaling feel right?
- Do CV1 and CV2 feel like faithful X/Y pad modulation inputs?
- Does Pulse1 gating feel useful, or should it become a mode/chord trigger?
- Does the switch short-press chord cycle feel like the original bee tap?
- Does switch hold make sense as open-gate-with-Pulse1 and mute-drone-without-Pulse1?
- Are the LED behaviors helpful enough for a card without artwork?
- Any control ranges that should be curved, limited, or swapped in a future revision.
