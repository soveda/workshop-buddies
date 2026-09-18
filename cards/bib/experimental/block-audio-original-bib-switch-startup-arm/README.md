# Workshop Block-Audio: Original Bib Switch Startup Guard Test

This isolated version starts from the passed original-Bib tap-delay build. It
includes the original Bib 64-frame DSP from the public
[Buddies repository](https://github.com/soveda/buddies_public), under
GPL-3.0-or-later. Its custom Workshop transport does not use `ComputerCard.h`;
hardware-map portions derive from ComputerCard v0.3.0 by Chris Johnson (MIT).

## Startup Guard

Some Workshop cards can briefly report Switch Down while their multiplexed ADC
input settles after reset. Previously that phantom state could toggle wavefold
on boot, producing the unwanted fizzy sound.

Switch actions are now disarmed at startup. They arm only after the switch has
been stably observed in Middle or Up. A real Down action then behaves as before:

- Drive page: Down toggles wavefold.
- Delay page: Down taps delay time.
- Mix page: hold Down for dub hold.

If the switch is physically Down during boot, it is ignored until released.
This deliberately prevents both false wavefold toggles and false dub hold.

All Main/X/Y page controls retain the passed tap-delay behaviour. LEDs 0 and 1
remain transport-error indicators.

## Build

```sh
cmake -S . -B build -G "Unix Makefiles"
cmake --build build
```

The output is `build/workshop_block_audio_original_bib_switch_startup_arm_test.uf2`.

## Hardware Test Protocol

1. Set Main to the Drive page (LED 2), with Switch Middle, then flash and
   reset the card at least ten times. The initial sound must always be normal
   soft clipping, never wavefold fizz. LEDs 0 and 1 must remain off.
2. Repeat resets with Switch Up selected. Again, wavefold must remain off.
3. Boot once with Switch Down held or selected. Release it to Middle, then
   press Down once in Drive mode. Only this deliberate press should enable
   wavefold; a second press should disable it.
4. Confirm Down still taps delay in LED 3 and holds dub in LED 5 after the
   switch has first been released or observed in Middle/Up.
5. Run audio for 30 minutes with repeated resets and switch actions. Report
   any startup fizz, unintended switch action, LED 0/1, click, instability,
   or reset requirement.
