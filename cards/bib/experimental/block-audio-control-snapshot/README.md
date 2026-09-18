# Workshop Block-Audio Driver: Control Snapshot Test

This is a standalone Pico SDK experiment for the Workshop Computer. It is a
second test version: the earlier block-audio passthrough remains unchanged.
Neither version includes or inherits from `ComputerCard.h`.

It borrows only the verified Workshop pin map, DAC word format, ADC correction,
and mux timing from ComputerCard v0.3.0, which is MIT licensed by Chris
Johnson. The purpose is to establish a reusable block-audio transport for cards
whose DSP does not fit the framework's one-sample callback model.

## What this version adds

The passed 48 kHz stereo transport and two-block core-1 passthrough are
unchanged. This build additionally snapshots the following at each 64-frame
block boundary and passes them with the audio block to core 1:

- Main, X, and Y, using ComputerCard's original mux scan and smoothing.
- Latching Switch position.
- Both CV inputs, using ComputerCard's alternating mux scan and smoothing.
- Rising pulse edges seen during the block.
- Audio/CV/pulse cable detection using the normalisation probe protocol.

The values are intentionally not connected to the audio processor yet. This is
therefore still an audio passthrough and a control transport proof, not Bib.

## Deliberate omissions

This is not a usable Bib card. It does not yet expose the snapshots as a public
framework API, implement CV/pulse outputs, apply audio normalisation, or run
any Bib DSP. Audio 2 is not normalled from Audio 1 in this build.

LED 0 lights if core 1 fails to consume input blocks in time. LED 1 lights if
core 0 needs an output block that core 1 has not produced. LED 2 blinks while
blocks flow and stays bright briefly after a Pulse In 1 rising edge. With the
switch Middle, LEDs 3-5 show Main, X, and Y. Switch Down lights LED 3; Switch
Up lights LED 4.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_controls_test.uf2`.

## Hardware Test Protocol

1. Flash the UF2, reset the Workshop Computer, and confirm LED 2 blinks.
   LEDs 0 and 1 should remain off after startup.
2. Patch a steady mono signal to Audio In 1 and listen at Audio Out 1. Confirm
   stable, clean passthrough without clicks, fizz, dropouts, or level drift.
3. Repeat using Audio In 2 and Audio Out 2. Then patch distinct signals into
   both inputs and confirm channels remain separate.
4. With the switch Middle, check LEDs 3-5 smoothly track Main, X, and Y.
   Move the switch Down and Up: LED 3 and LED 4 respectively should light.
   Audio must remain unchanged in all switch positions.
5. Send repeated gates to Pulse In 1. LED 2 should brighten briefly for each
   rising edge, without clicks or transport error LEDs.
6. Patch and unpatch each audio, CV, and pulse input while audio runs. Audio
   must remain stable. The normalisation state is passed to core 1 but is not
   yet intentionally mapped to audio behaviour.
7. Leave both channels running for at least 30 minutes. LEDs 0 or 1 indicate a
   transport failure; report whether either lights and whether a reset is ever
   needed.

Do not use this transport with a live performance or as a replacement for the
Bib release candidate. It is the prerequisite for a later exact block-DSP port.
