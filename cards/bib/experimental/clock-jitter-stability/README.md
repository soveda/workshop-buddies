# Bib 1.1.2-beta clock stability test

This separate test extends the [1.1.1-beta reacquisition test](../clock-reacquisition/README.md).
Both tests and the 1.1.0-beta release remain available for comparison.
The original DSP, reverb, audio levels and musical control mappings are unchanged.

Flash `uf2/Bib-1.1.2-beta-clock-stability-test.uf2` and reset with Switch Middle.
The [release README](../../README.md) describes the complete controls.

## Changes

- Pulse In 1 edges carry a sample-counter timestamp from the audio ISR into
  the existing control snapshot. A steady 125 ms clock now measures 6000 samples
  regardless of where the edge falls within a 64-frame DSP block.
- Small period fluctuations use a fractional fixed-point 1/16 smoothing step.
  Changes larger than 128 samples jump once accepted; large rate changes still
  need two consistent intervals, as in test 1.1.1.
- Division selection retains the current musical ratio until another choice
  is clearly closer: a margin of 1/32 of the division spacing, or 16 samples.
  This prevents repeated read-head jumps near a division boundary. The retained
  ratio still follows tempo changes, and moving X still selects other ratios.
- Unpatched Pulse In 1 normalisation-probe edges are ignored.

Processing remains 48 kHz, 64 frames, 192 MHz, with the same DMA transport.
The ISR only adds a counter increment and edge timestamp storage; smoothing
and division selection run on the DSP core. No extra USB service is enabled.
This is not pitch-preserving time stretching: deliberate delay-time changes
can still bend pitch. Hardware testing is required to check the reported speed-up.

## Comparative hardware test

1. Test the two UF2s separately with identical patching. Use audio with an
   obvious stable pitch or a repeating phrase. Patch the clock only to Pulse
   In 1; leave CV 1/2 unpatched. Keep Switch Middle throughout the clock tests.
2. Set Drive delay send clockwise above centre, choose Delay, and use moderate
   Y feedback. Move X through noon to establish pickup, then hold it still.
3. At 120 BPM, start with quarter-note clock pulses. Wait for LED 4 to show
   lock, then listen after one second of settling.
4. Change to sixteenth-note pulses without moving X. Confirm relocking and
   approximately the same repeat spacing and settled pitch. Change back.
5. Repeat after reset, starting directly with sixteenths. Slowly sweep X
   around 46% travel, just below noon, then hold near a division boundary.
   Test 1.1.2 should stop repetitive warbling, speeding/slowing or clicks caused
   by the old 93/94-block measurements. Physical pickup means the boundary may
   not occur at exactly that knob position; find it by listening.
6. Repeat with a different tempo, for example 123 BPM. Make a deliberate tempo
   change and move X across several divisions. Timing should follow; transient
   pitch bends during real delay changes remain expected.
7. Stop the clock, check that delay holds, move X to restore free timing, then
   restart. Check multitap and page pickup, then tape speed on Switch Up and
   shimmer on Reverb. Run continuously for at least 15 minutes.

If a sustained speed-up remains, note firmware version, clock BPM/division,
switch position, X region, whether it occurs after settling or only during
changes, and whether the dry signal or just delayed audio changes.

## Build and checks

```sh
export PICO_SDK_PATH=/path/to/pico-sdk
cmake -S . -B build -G "Unix Makefiles"
cmake --build build -j2
c++ -std=c++17 -Wall -Wextra tests/clock_test.cpp -o /tmp/bib-clock-stability-test
/tmp/bib-clock-stability-test
```

Build outputs stay ignored. Tests exercise the same clock helpers as firmware,
including every possible edge position in a DSP block and the demonstrated
division-boundary instability. Workshop changes were AI-assisted.

Original Bib: [Plinky Synth Buddies](https://github.com/plinkysynth/buddies_public/tree/main/sw/src/bib).
See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md), [LICENSE](LICENSE) and
the bundled `licenses` folder. Keep `NOTICE.txt` with redistributed UF2s.
