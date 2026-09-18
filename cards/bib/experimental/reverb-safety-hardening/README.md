# Bib Reverb Safety Hardening Test

This is a self-contained test copy of the current Bib 1.0.0-rc1 source. The
release files in `cards/bib/` are unchanged.

## Change under test

Only the original Bib reverb tank is hardened. Its accumulator, damping
state, and feedback output are bounded at +/-65535 before nonlinear or
high-gain fixed-point operations. This prevents signed-integer overflow from
an unusually large reverb transient while leaving normal signal levels,
controls, delay behaviour, and reverb parameters unchanged.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The resulting file is `build/bib_reverb_safety_test.uf2`.

## Test protocol

1. Confirm the normal four modes, delay tapography, Pulse In 1 clocking, tape
   page, and freeze behave identically to the current release candidate.
2. In Mode 2, compare the normal reverb sound at low and mid X/Y settings to
   the release candidate. There should be no audible difference.
3. With a loud, sustained input, set Mode 2 X and Y fully clockwise and leave
   it running for several minutes. Confirm continuous audio, responsive knobs
   and LEDs, and no reset requirement.
4. Set Switch Up while still on Mode 2 and take X fully clockwise to apply
   maximum shimmer. Exercise the input level, reverb send, and decay through
   their full ranges. Expect saturation at extreme combinations, but never a
   stuck card, uncontrolled noise, or loss of controls.
5. Return all controls to ordinary ranges. Confirm the tank recovers cleanly
   and that delay and dry audio remain stable.
