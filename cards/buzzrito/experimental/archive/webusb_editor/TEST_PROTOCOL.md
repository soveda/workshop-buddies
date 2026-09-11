# WebUSB Editor Test Protocol

> Status: failed. Retained as a record of the test plan and observed runtime
> conflict; do not run this image as a normal Buzzrito card.

## Setup

1. Build this archived source locally and copy its generated UF2 to the
   Workshop Computer.
2. Connect the card's USB-C port directly to a Chromium-family browser.
3. Open <https://plinkysynth.com/docs/buzzrito-manual/>, choose **Connect**, and
   select `Workshop Buzzrito WebUSB`.

## Checks

1. The editor connects without an endpoint error and displays the seven
   original Buzzrito preset values after its initial read.
2. With the browser idle, check Main, Audio/CV In 1, X, Y, CV1/CV2, Pulse1,
   switch-down chord selection, and switch-up motion. They should match the
   passed motion fallback's sound and behavior.
3. Drag the editor XY point. The card should follow the editor point, including
   the distinct sub-heavy lower region and comb-heavy right side. LEDs show the
   editor point while it owns the pad.
4. Move physical X or Y. Hardware control should immediately resume, with no
   click, stutter, or persistent editor takeover. Patch CV1 or CV2 and confirm
   it also returns to hardware control.
5. Change **Saw Level**, **Sub Level**, **Comb Depth**, and **Comb Pitch**.
   Each should affect the current sound smoothly. Test Reset, Export, and
   Import; disconnect/reconnect should return the edited in-RAM values.
6. Move Wobble, Glide, BOC, and Noise controls. Wobble is deliberately bounded;
   Glide, BOC, and Noise should not introduce noise, wind-like motion, or
   instability in this experiment.
7. Power-cycle. Preset edits should return to the compiled defaults. Persistent
   editor saves are intentionally out of scope for this build.

## Pass Criteria

- Browser connection and full preset transfer work reliably.
- Editing never causes audio dropouts, unstable pitch/character, or rapid LED
  flicker.
- Physical controls always reclaim the virtual pad predictably.
- The hardware-only baseline remains indistinguishable from the passed fallback.
