# Buzzrito Experiments

## Active Fallback

`webusb_modal_editor/` is the passed experimental fallback. It provides stable
normal performance, WebUSB editor boot with Switch Up, and saved preset data.
Use its source as the starting point for every later sound parameter experiment.

`parameter_trials/glide/` is the current source-only glide test. Its timing
notes establish the required 64-sample conversion for any later BOC or noise
experiment.

`PERFORMANCE_NOTES.md` records the real-time constraints for later USB, MIDI,
BOC, and noise work.

## Archive

`archive/` contains superseded experiments retained for reference only:

- `switch_up_motion/`: early gesture recorder work, now promoted into the
  stable performance firmware.
- `usb_midi_stability/`: always-on and modal USB MIDI scheduling trials.
- `webusb_editor/`: the original always-on WebUSB trial that disturbed audio.
