# Workshop Blueberry

**First-pass test firmware.** Workshop Blueberry is a Blueberry-inspired,
monophonic quantised keyboard recorder for the Workshop Computer. Patch one
output from the Workshop System's **4 Voltages** module to Audio In 1. The card
learns its no-button voltage after reset, then treats changing voltages from
the four buttons and their combinations as a small musical keyboard.

This is a new Workshop implementation inspired by the original
[Blueberry](https://github.com/plinkysynth/buddies_public/tree/main/sw/src/blueberry),
not a direct firmware port.

## First-Pass Behaviour

| Control | Function |
| --- | --- |
| Audio In 1 | 4 Voltages keyboard input. Leave all buttons released during the first 128 ms after reset. |
| Main | Blueberry-style persistent transposition. Turn past 3 o'clock for one upward notch, return through centre, then repeat. Turn past 9 o'clock for one downward notch. A notch alternates fifth/octave movement exactly like Blueberry's buttons. |
| Switch Middle | Free play: live keyboard voltage is quantised to C major pentatonic. |
| Switch Down | Record while held. The captured loop lasts exactly as long as the hold. |
| Switch Up | Play the recorded loop. |
| Pulse In 1 | Clock. While patched in Up, each rising edge advances one recorded note event, following original Blueberry clock behaviour. Without a clock, playback uses the captured timing. |
| CV Out 1 | Quantised pitch, using Workshop output calibration. |
| Pulse Out 1 | Gate: high while a keyboard note or recorded event is active. |
| Pulse Out 2 | Brief pulse at each loop restart. |

Audio In 2, CV In 1/2, Audio Outs, and CV Out 2 are unused in this first pass.

## LEDs

| LED | Meaning |
| --- | --- |
| 0 | Input calibration present. It blinks while measuring the startup baseline. |
| 1 | Loop playback is active in Switch Up. |
| 2 | Recording while Switch Down is held. |
| 3 | Pulse In 1 is patched. |
| 4 | Gate is high. |
| 5 | Current quantised pitch brightness. |

## Important Limits

- Loop data is held in RAM and clears at reset/power-off.
- The first pass stores up to 512 changes of pitch or gate. Steady held notes
  use one event regardless of their duration, so typical phrases can be long.
- The scale is fixed to Blueberry's default major pentatonic for now.
- Input calibration is available in ComputerCard 0.4.0. When no calibration
  record is present, the card still works with the framework's approximate
  voltage conversion but LED 0 remains off.

## Build

```sh
export PICO_SDK_PATH=/path/to/pico-sdk
cmake -S cards/blueberry -B /private/tmp/workshop-blueberry -G "Unix Makefiles"
cmake --build /private/tmp/workshop-blueberry -j2
```

The build creates `workshop_blueberry.uf2`.

## Attribution And Licence

Workshop Blueberry is MIT licensed. It includes the MIT-licensed
`ComputerCard.h` 0.4.0 framework by Chris Johnson. Its interaction model is
inspired by the MIT-licensed Blueberry software in the
[Buddies public repository](https://github.com/plinkysynth/buddies_public/tree/main/sw/src/blueberry).
No original Blueberry source, graphic design, or hardware files are included.
See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
