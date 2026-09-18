# Workshop Block-Audio: Bib Delay Feedback Cap Test

This separate experiment starts from the startup-guard, wavefold-cap, and
knob-pickup version. It includes original Bib 64-frame DSP from the public
[Buddies repository](https://github.com/soveda/buddies_public), under
GPL-3.0-or-later. The custom Workshop transport does not use `ComputerCard.h`;
hardware-map portions derive from ComputerCard v0.3.0 by Chris Johnson (MIT).

## Delay Feedback Safety Cap

At the top of the Delay Feedback control, Bib's original feedback transform
reaches a non-decaying self-oscillation region. Residual delay data can then
become a low hum even after both audio inputs are unpatched.

The DSP-facing Delay Feedback value is now capped at `2490`, close to the
previous fixed stable profile (`2500`). It retains a long, musically useful
feedback tail, but prevents the zero-decay endpoint. The physical Y knob and
its saved pickup value still cover their full range; the upper part now maps
to this stable maximum.

Startup switch arming, wavefold cap, Main page selection, X/Y soft pickup,
tap delay, and dub hold are otherwise unchanged. LEDs 0 and 1 remain
transport-error indicators.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_guard_wavefold_pickup_delay_feedback_cap_test.uf2`.

## Hardware Test Protocol

1. Flash and reset repeatedly. Confirm startup remains normal and LEDs 0/1
   stay off.
2. In the Delay page (LED 3), set an audible delay send in LED 2, then sweep
   Y from minimum to maximum. The upper range should produce a long tail but
   must not become a persistent low hum or self-oscillate.
3. Stop the input and unpatch both audio inputs at every Y position. All
   residual sound must decay to silence without a repeating low tone.
4. Test a very short, middle, and long delay time at maximum Y. Confirm no
   time setting causes a runaway loop, noise burst, or transport LED.
5. Confirm page pickup, wavefold, tap delay, and dub hold still work. Run a
   delay-heavy patch for 30 minutes; report any persistent hum, instability,
   freeze, or reset requirement.
