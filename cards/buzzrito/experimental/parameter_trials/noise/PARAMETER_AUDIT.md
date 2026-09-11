# Original Parameter Audit

The active modal WebUSB editor persists all ten fields of each original
`buzzypreset`. The normal Workshop renderer currently uses only the stable
subset below.

| Original field | Current state | Next experiment |
| --- | --- | --- |
| `spread` | Active | No change needed. |
| `glide` | Active | Hardware-passed 64-sample pitch-delta slew. |
| `boc_amount` | Active, depth-limited | Original 64-sample interpolation noise; first trial limits depth to 1/8. |
| `wobble_amount` | Active, simplified | Current deterministic per-saw detune is deliberately restrained. |
| `wobble_speed` | Active, simplified | Controls the restrained deterministic wobble rate. |
| `saw_level` | Active | Current output level is safety-limited to preserve knob-space audibility. |
| `sub_level` | Active | Current output level is safety-limited to preserve knob-space audibility. |
| `noise_level` | Active, depth-limited | Independent 48 kHz stereo pink-noise mix; first trial limits depth to 1/16. |
| `comb_depth` | Active | Retained in the stable signed-feedback comb. |
| `comb_mul` | Active | Retained as the comb pitch ratio. |

This trial was copied from the hardware-passed BOC source. The earlier
versions remain unchanged; generated firmware is kept out of Git.
