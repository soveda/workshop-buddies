# Workshop Block-Audio: Original Bib Controls Test

This is a separate experiment built from the passed original-Bib DSP block
test. It runs original Bib `process_bib()` DSP on 64-frame stereo blocks at
48 kHz, with the public [Buddies source](https://github.com/soveda/buddies_public)
included under GPL-3.0-or-later. The Workshop block transport is independent of
`ComputerCard.h`; its hardware-map portions are derived from ComputerCard
v0.3.0 by Chris Johnson (MIT).

## Controls

Main is the virtual four-position capacitor slider. Each quarter selects a
Bib control page, shown by one lit LED:

| Main range | LED | X | Y |
| --- | --- | --- | --- |
| Fully CCW to about 10:30 | 2 | Drive | Delay send; centre is off, clockwise is normal delay, counter-clockwise is ping-pong |
| About 10:30 to 12:30 | 3 | Delay time | Delay feedback |
| About 12:30 to 2:30 | 4 | Reverb send | Reverb feedback |
| About 2:30 to fully CW | 5 | Wet/dry mix; centre is balanced | Output level |

Each page retains its X/Y values while another page is selected. This pass has
no soft takeover yet: after choosing a page, its values follow the current X/Y
positions immediately. Shimmer, touch-slider behaviour, the switch-as-spider
actions, pulse clocking, and CV modulation are intentionally not implemented
in this test.

LEDs 0 and 1 remain error indicators for input overrun and output underrun.
Neither should light during normal use.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_controls_test.uf2`.

## Hardware Test Protocol

1. Flash the UF2 and reset. Confirm LEDs 0 and 1 remain off. Exactly one of
   LEDs 2-5 should be lit, reflecting Main's current quarter.
2. Patch a steady source to Audio In 1. Move Main through all four quarters.
   Confirm only the mode LED changes until X or Y is moved.
3. In page 1, sweep X for a clean drive range and Y through the centre point.
   Centre should remove the delay send; the two sides should give normal and
   ping-pong delay directions.
4. In page 2, sweep X from short to long delay and Y from low to high
   feedback. Check for smooth, stable changes and no lock-up at either end.
5. In page 3, sweep X and Y independently. Reverb should increase smoothly;
   high feedback must remain stable rather than becoming harsh digital noise.
6. In page 4, sweep X through the centre and Y from low to high. Confirm the
   mix changes smoothly and output level stays usable without digital clicks.
7. Return to each prior page. Its last X/Y values should be restored until X
   or Y is moved again. Check all LEDs 2-5 map to their respective pages.
8. While audio runs, patch/unpatch CVs and send pulse gates. They must not
   change audio or destabilise it in this pass. Switch positions are also
   intentionally inactive.
9. Run an active delay/reverb patch for 30 minutes. Any LED 0/1, dropout,
   changing pitch, burst of noise, or reset requirement is a failure.
