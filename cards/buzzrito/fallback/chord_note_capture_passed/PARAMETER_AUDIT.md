# Original Parameter Audit

The active modal WebUSB editor persists all ten fields of each original
`buzzypreset`. The normal Workshop renderer currently uses only the stable
subset below.

| Original field | Current state | Next experiment |
| --- | --- | --- |
| `spread` | Active | No change needed. |
| `glide` | Active | Hardware-passed 64-sample pitch-delta slew. |
| `boc_amount` | Active, full-depth passed | Original 64-sample interpolation noise; no depth cap. |
| `wobble_amount` | Active, full-depth staggered passed | Per-saw interpolation noise, evaluated once per 64 samples. |
| `wobble_speed` | Active, original path | Controls the original 750 Hz block-rate interpolation speed. |
| `saw_level` | Active | Current output level is safety-limited to preserve knob-space audibility. |
| `sub_level` | Active | Current output level is safety-limited to preserve knob-space audibility. |
| `noise_level` | Active, quarter-depth passed | Independent 48 kHz stereo pink-noise mix; calibration limits depth to 1/4. |
| `comb_depth` | Active | Retained in the stable signed-feedback comb. |
| `comb_mul` | Active | Retained as the comb pitch ratio. |
| Chord modes 1-4 | Active, original note memory passed | Retains 1-4 stable pitches at the original 750 Hz control cadence. |

This trial was copied from the hardware-passed BOC source. The earlier
versions remain unchanged; generated firmware is kept out of Git.
