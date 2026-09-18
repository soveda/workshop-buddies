# Workshop Block-Audio: Bib Startup Guard and Wavefold Cap Test

This test combines the switch startup guard with a gentler maximum wavefold
drive. It uses original Bib 64-frame DSP from the public
[Buddies repository](https://github.com/soveda/buddies_public) under
GPL-3.0-or-later. Its custom Workshop transport does not use `ComputerCard.h`;
hardware-map portions derive from ComputerCard v0.3.0 by Chris Johnson (MIT).

## New Behaviour

Switch actions arm only after the switch has stably settled in Middle or Up.
This prevents the transient Down reading at reset from enabling wavefold. If
Down is physically selected during boot, release it before the first intended
action.

In the Drive page (LED 2), Down still toggles original Bib wavefold. Normal
soft-clip Drive remains its full original range. When wavefold is enabled,
Drive is capped at `3072` internally: the lower and middle folded range is
unchanged, while the destructive top quarter is limited to roughly an 8x
gain region rather than the original extreme maximum.

Tap delay and dub hold retain their passed behaviour. LEDs 0 and 1 remain
transport-error indicators.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_switch_startup_arm_wavefold_cap_test.uf2`.

## Hardware Test Protocol

1. With Switch Middle and Main in Drive mode, reset at least ten times. The
   startup sound must consistently be normal soft clipping with no wavefold
   fizz. LEDs 0 and 1 must remain off.
2. After startup, press Down once in Drive mode. Sweep X from minimum to
   maximum. Expect a clear folding character that stays usable at the top;
   it should be less harsh than the previous wavefold build.
3. Press Down again and repeat the X sweep. Normal soft clipping must return
   and its full Drive range must be unchanged from the passed tap-delay build.
4. Confirm switch-down tap delay and dub hold still work after boot. Test a
   boot with Down selected: it should do nothing until released, then respond
   to the next deliberate Down action.
5. Run an active signal for 30 minutes, including repeated resets and full X
   wavefold sweeps. Report any fizz when wavefold is off, transport LEDs,
   clicks, instability, freeze, or reset requirement.
