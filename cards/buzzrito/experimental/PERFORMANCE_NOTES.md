# Future Performance Notes

Apply these rules to later BOC, noise, WebUSB, and MIDI experiments without
changing the passed normal-performance renderer until each change is hardware
tested.

The 64-sample glide trial has passed hardware testing. Its source is a stable
starting point for the BOC trial, which has also passed hardware testing. The
BOC source is the stable starting point for any later isolated noise experiment.

- Avoid division, dynamic allocation, logging, and flash writes in the 48 kHz
  audio callback. Precompute coefficients at a control rate or use fixed-point
  shifts and lookup tables.
- Keep the audio interrupt and the lookup tables it needs in RAM where memory
  budget permits, using `__not_in_flash` / `__not_in_flash_func`. This avoids
  flash-XIP stalls during timing-sensitive rendering.
- If persistent USB MIDI is revisited, move USB polling, knob/CV reads, and UI
  work to the non-audio core. Send compact control snapshots to the audio core
  through a lock-free single-producer/single-consumer ring buffer.
- The audio core must consume the latest complete snapshot without waiting for
  USB or UI work. A dropped stale control update is preferable to blocking the
  48 kHz callback.
- The existing modal WebUSB approach remains the fallback. Earlier always-on
  USB experiments disturbed oscillator and gesture behavior, so a dual-core
  handoff must prove stable before it replaces the modal design.
