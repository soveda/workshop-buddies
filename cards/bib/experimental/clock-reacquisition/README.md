# Bib 1.1.1-beta clock reacquisition test

This is a separate test based on Bib 1.1.0-beta. The release source and
`uf2/Bib-1.1.0-beta.uf2` in the parent card folder remain the fallback.
The original Bib DSP, DMA transport and control mapping are unchanged.

Flash `uf2/Bib-1.1.1-beta-clock-test.uf2`. See the
[release instructions](../../README.md) for the complete controls and attribution.

Previously, a clock locked to quarter notes rejected sixteenth-note periods
because they were outside its 2:1 rate-change guard. This test accepts such a
change after two consecutive intervals agree within 2% or one 64-frame block,
whichever is larger. An isolated large timing error retains the previous rate.
Clock periods must still be at least 50 ms and less than two seconds.

X selects the delay nearest its stored time from Bib's 3/4, straight and dotted
choices, scaled by powers of two. At the same X, a quarter-to-sixteenth change
should therefore give approximately the same delay time after relocking.
Clock measurement has 1.33 ms resolution, so small differences remain possible.
Changing X or clock tempo can briefly bend the delayed audio's pitch as Bib
moves the read head. This is not a pitch-preserving time-stretch effect.
Use Switch Middle: Switch Up deliberately controls tape speed instead.

## Hardware test

1. Reset with Switch Middle, no CV patched and a steady audio signal on Audio
   In 1. Enable delay send on Drive, select Delay and set X near noon with
   moderate feedback. Confirm normal unclocked sound.
2. Feed Pulse In 1 quarter notes at 120 BPM (500 ms). LED 4 should light when
   locked. Listen to the repeat spacing after settling.
3. Keep X still and change the clock to sixteenths (125 ms). After two complete
   matching new intervals, repeat spacing should be approximately the same.
   A lasting fourfold speed/pitch change is a failure; a brief transition is
   possible. Wait a second for the DSP smoothing to settle.
4. Change back to quarters. LED 4 may go out during the slower clock's
   reacquisition, then return. Confirm the same repeat spacing after settling.
5. Turn X across its range while clocked. Delay should move between musical
   divisions/multiples. Pitch bends during these time changes are expected.
6. Stop the clock. The delay should hold its last time; move X deliberately
   to resume free timing. Restart the clock and confirm it locks again.
7. Repeat steps 2--4 starting with sixteenths after reset, and with another
   tempo. Then check multitap, page pickup, tape speed and shimmer against
   the fallback. Run audio for at least 15 minutes.

If the reported sustained speed/pitch difference remains after relocking,
record the clock tempo, X position, switch position and whether CV is patched.
This change fixes a verified tracking bug; hardware testing is still needed
to establish whether it accounts for the reported audible behaviour.

## Build and software checks

```sh
export PICO_SDK_PATH=/path/to/pico-sdk
cmake -S . -B build -G "Unix Makefiles"
cmake --build build -j2
c++ -std=c++17 -Wall -Wextra -I. tests/clock_test.cpp -o /tmp/bib-clock-test
/tmp/bib-clock-test
```

The firmware build produces `build/bib_workshop.uf2`. Build directories are
ignored. The test uses the same clock helper and quantisation code as firmware.
Workshop-specific changes were AI-assisted. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)
and [LICENSE](LICENSE) for source attribution and licensing.
