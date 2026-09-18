# Bib Control-Rate Work Reduction Test

This self-contained test copy is based on the current Bib 1.0.0-rc1 release.
It does not replace either the release or the separate reverb-safety test.

## Change under test

The original control mapping, page pickup, CV mapping, and LED updates now run
once every eight audio samples, at 6 kHz rather than 48 kHz. Audio processing,
delay interpolation, reverb processing, output limiting, switch freeze, and
Pulse In 1 edge measurement remain at the 48 kHz audio rate.

A Switch Down edge is latched until the next control tick, so a short Z press
still reaches the appropriate mode. Pulse In 1 remains per sample because a
rising-edge event is only present for a single callback.

This reduces non-DSP work in the callback while preserving the existing release
DSP and control behaviour as closely as possible. The separate safety-test
clamps are intentionally not included, so timing can be evaluated in isolation.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The resulting file is `build/bib_control_rate_test.uf2`.

## Test protocol

1. Flash the test UF2 after noting the sound of the current release. Confirm
   startup, audio pass-through, and all four Main pages behave normally.
2. Turn X, Y, and Main slowly and quickly through their full ranges. Confirm
   no audible stepping, missed pickup, LED lag that feels distracting, or
   parameter jump is introduced.
3. Test Z in each applicable mode: wavefold toggle, multi-tap entry, and
   freeze. Short presses must register once; a held freeze must start and end
   immediately.
4. Feed a regular clock to Pulse In 1. Confirm lock, clock loss handoff, and
   manual/tapped delay timing remain stable.
5. In Mode 2, exercise moderate reverb send and decay with a sustained input.
   The ordinary sound should match the release candidate.
6. Leave a musically typical patch running for at least ten minutes, moving
   controls and switch pages occasionally. Confirm audio and controls remain
   stable with no reset requirement.
