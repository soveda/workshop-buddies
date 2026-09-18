# Workshop Block-Audio: Original Bib DSP Test

This standalone Pico SDK experiment is the next stage after the passed
block-audio transport and control-normalisation tests. It does not include or
inherit from `ComputerCard.h`.

It runs the original Bib `process_bib()` function on core 1 over its native
64-frame, 48 kHz interleaved stereo blocks. The original Bib delay, reverb,
and fixed-point lookup-table sources are included here from the public
[Buddies repository](https://github.com/soveda/buddies_public), under its
GPL-3.0-or-later licence. The only source adaptations are the small Workshop
build compatibility header and C/C++ table linkage guard; the DSP algorithm is
otherwise unchanged.

The Workshop hardware map, ADC DNL correction, mux cadence, and MCP4822 DAC
format originate in ComputerCard v0.3.0 by Chris Johnson (MIT).

## Fixed DSP profile

This is deliberately not a control-mapping test. Main, X, Y, CVs, switch, and
pulses retain the prior diagnostic LED behaviour, but do not alter audio. The
audio path uses these fixed original-Bib units:

- Drive: `2048` (unity region)
- Delay send: `2048`; delay time: `16384`; feedback: `2500`
- Reverb send: `1000`; feedback: `2600`; shimmer: `0`
- Wet/dry mix: `0`; output level: `2048`

This creates a moderate stereo delay/reverb response without testing the
extreme parameter ranges yet. Audio inputs are muted after normalisation
detects that they are unpatched, so the probe sequence is not audible.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_dsp_test.uf2`.

## Hardware Test Protocol

1. Flash the UF2 and reset the Workshop Computer. After startup, LEDs 0-2
   must remain off.
2. Patch a steady source to Audio In 1 and monitor both outputs. Confirm a
   stable processed sound with an audible, moderate delay/reverb tail. There
   must be no constant white noise, fizz, clicking, or periodic dropout.
3. Repeat for Audio In 2. Patch distinct material to both inputs and confirm
   the processed stereo result remains stable.
4. Stop the input and listen for the effect tail to decay naturally. Then
   unpatch either audio input: its signal must fall silent without probe noise.
5. Move Main, X, Y, switch, patch CVs, and send Pulse In 1 while audio runs.
   The prior diagnostic LEDs should respond, but the fixed sound profile must
   not change and LEDs 0 or 1 must never light.
6. Run stereo audio for at least 30 minutes. Any LED 0/1 illumination,
   distortion, loss of audio, reset requirement, or changing effect character
   is a failed transport/DSP timing result and should be reported.

This is an experimental build, not a release replacement for the established
per-sample Bib firmware.
