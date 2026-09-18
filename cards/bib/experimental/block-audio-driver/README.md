# Workshop Block-Audio Driver: First Pass

This is a standalone Pico SDK experiment for the Workshop Computer. It does
not include or inherit from `ComputerCard.h`.

It borrows only the verified Workshop pin map, DAC word format, ADC correction,
and mux timing from ComputerCard v0.3.0, which is MIT licensed by Chris
Johnson. The purpose is to establish a reusable block-audio transport for cards
whose DSP does not fit the framework's one-sample callback model.

## What this version does

- Captures stereo audio at 48 kHz with the Workshop ADC/DMA arrangement.
- Keeps the existing four-state external mux cadence.
- Queues MCP4822 stereo DAC writes at 48 kHz through SPI DMA.
- Collects 64 stereo frames on core 0, then passes complete blocks to core 1
  through a lock-free four-slot ring.
- Runs a deliberately trivial core-1 block processor: stereo passthrough.
- Returns processed audio two 64-frame blocks later. Expected latency is about
  2.7 ms, plus the normal analogue converter delay.

## Deliberate omissions

This is an audio-transport proof, not a usable Bib card. It does not yet expose
pots, switch, CV, pulse events, normalisation detection, CV outputs, LEDs as a
program API, or any Bib DSP. Use both audio inputs for the stereo test; Audio
2 is not normalled from Audio 1 in this build.

LED 0 lights if core 1 fails to consume input blocks in time. LED 1 lights if
core 0 needs an output block that core 1 has not produced. LED 2 slowly blinks
while complete input blocks are being captured. LEDs 3-5 are unused.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_passthrough.uf2`.

## Hardware Test Protocol

1. Flash the UF2, reset the Workshop Computer, and confirm LED 2 blinks.
   LEDs 0 and 1 should remain off after startup.
2. Patch a steady mono signal to Audio In 1 and listen at Audio Out 1. Confirm
   stable, clean passthrough without clicks, fizz, dropouts, or level drift.
3. Repeat using Audio In 2 and Audio Out 2. Then patch distinct signals into
   both inputs and confirm channels remain separate.
4. Move all three pots and the switch while listening. They are intentionally
   not mapped yet, so audio should remain stable and unchanged.
5. Leave both channels running for at least 30 minutes. LEDs 0 or 1 indicate a
   transport failure; report whether either lights and whether a reset is ever
   needed.

Do not use this transport with a live performance or as a replacement for the
Bib release candidate. It is the prerequisite for a later exact block-DSP port.
