# Buzzrito Experiments

## Active Fallback

`webusb_modal_editor/` is the passed experimental fallback. It provides stable
normal performance, WebUSB editor boot with Switch Up, and saved preset data.
Use its source as the starting point for every later sound parameter experiment.

`parameter_trials/glide/` is the current source-only glide test. Its timing
notes establish the required 64-sample conversion for any later BOC or noise
experiment.

`parameter_trials/boc/` is the separate, depth-limited BOC test branched from
the hardware-passed glide version. It has passed hardware testing and is the
starting point for any later isolated noise work.

`parameter_trials/noise/` is the separate, depth-limited pink-noise test
branched from the hardware-passed BOC version. It has passed hardware testing.

`parameter_trials/boc_half_depth/` calibrates the BOC cap at one half of the
original range while retaining the passed noise implementation.

`parameter_trials/boc_full_depth/` separately restores the original full BOC
range for comparison with the half-depth calibration. It has passed hardware
testing and is the preferred BOC range.

`parameter_trials/noise_quarter_depth/` calibrates pink noise at one quarter
of the original range while retaining passed full-depth BOC. It has passed and
is the selected maximum noise range.

`parameter_trials/noise_eighth_depth/` is retained as a too-subtle calibration
reference.

`parameter_trials/original_wobble_quarter/` separately trials original seeded
per-saw wobble at one quarter depth, with its state advanced at 750 Hz.

`parameter_trials/original_wobble_half/` raises that original wobble trial to
one half depth after quarter depth proved too subtle.

`parameter_trials/original_wobble_full_staggered/` restores full wobble depth
and spreads its 16 block-rate updates across each 64-sample block to address
the maximum-X distortion observed in the half-depth trial. It has passed
hardware testing, including that maximum-X stress case.

`parameter_trials/pitch_cv_calibrated/` derives from that passed version and
tests Audio/CV In 1 at C1ZZL3's hardware-proven 341 counts per volt, converted
to the millivolt pitch units expected by the original Buzzrito renderer.

`parameter_trials/chord_note_capture/` replaces the Workshop-only fixed chord
intervals with the original Buzzrito note-memory behavior for chord modes 1-4.
It has passed hardware testing and was promoted to `cards/buzzrito/`. Its
unchanged passed source is retained in `fallback/chord_note_capture_passed/`.

`PERFORMANCE_NOTES.md` records the real-time constraints for later USB, MIDI,
BOC, and noise work.

## Archive

`archive/` contains superseded experiments retained for reference only:

- `switch_up_motion/`: early gesture recorder work, now promoted into the
  stable performance firmware.
- `usb_midi_stability/`: always-on and modal USB MIDI scheduling trials.
- `webusb_editor/`: the original always-on WebUSB trial that disturbed audio.
