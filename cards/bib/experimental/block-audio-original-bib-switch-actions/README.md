# Workshop Block-Audio: Original Bib Switch Actions Test

This isolated version extends the passed original-Bib controls experiment. It
runs the original Bib `process_bib()` DSP in 64-frame blocks, using the public
[Buddies repository](https://github.com/soveda/buddies_public) under
GPL-3.0-or-later. The custom Workshop block transport does not use
`ComputerCard.h`; its hardware-map portions derive from ComputerCard v0.3.0 by
Chris Johnson (MIT).

Main and X/Y retain the four control pages from the prior test:

| Main range | LED | X | Y |
| --- | --- | --- | --- |
| Fully CCW to about 10:30 | 2 | Drive | Delay send |
| About 10:30 to 12:30 | 3 | Delay time | Delay feedback |
| About 12:30 to 2:30 | 4 | Reverb send | Reverb feedback |
| About 2:30 to fully CW | 5 | Wet/dry mix | Output level |

## New Switch Down Actions

Switch Down is debounced for three 64-frame blocks before acting. Switch Up
and Middle have no audio action in this version.

- In the Drive page (LED 2), each distinct Down press toggles Bib's original
  wavefold mode. Press again to return to normal soft clipping.
- In the Mix page (LED 5), hold Down for the original dub gesture: live drive
  and delay send are muted and delay feedback is set to maximum. Releasing
  Down restores the saved page settings.

The original tap-delay and pressure-controlled shimmer gestures are not added
yet, because a simple switch cannot represent their original touch data
without choosing a deliberate Workshop-specific interaction.

LEDs 0 and 1 remain input-overrun and output-underrun indicators. Neither
should light.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_switch_actions_test.uf2`.

## Hardware Test Protocol

1. Flash the UF2 and reset. Confirm LEDs 0 and 1 remain off; Main selects one
   lit page LED from 2-5 as before.
2. Confirm all four Main/X/Y pages retain the stable, working controls from
   the previous version before testing the switch.
3. Select LED 2, patch a steady source, and press Switch Down once. Expect a
   clear change from soft clipping to the more aggressive wavefold character.
   Keep holding it: the sound must stay stable and not toggle repeatedly.
   Release and press again; the original clipping character must return.
4. Select LED 5 and create a noticeable delay with the other pages. Hold
   Switch Down: fresh input should be removed while the delay loop continues
   at maximum feedback. Release Down: live input and the saved Drive/Send/
   Feedback settings must return immediately and cleanly.
5. Change pages, move controls, patch/unpatch CVs, and send pulse gates while
   audio runs. In this version, none should alter the new switch actions or
   destabilise audio.
6. Run an active delay/reverb patch for 30 minutes, including repeated switch
   presses and holds. Any LED 0/1, click burst, changing pitch, freeze, or
   reset requirement is a failed result.
