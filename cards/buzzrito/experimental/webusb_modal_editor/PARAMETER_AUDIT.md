# Original Parameter Audit

The active modal WebUSB editor persists all ten fields of each original
`buzzypreset`. The normal Workshop renderer currently uses only the stable
subset below.

| Original field | Current state | Next experiment |
| --- | --- | --- |
| `spread` | Active | No change needed. |
| `glide` | Not implemented | Add pitch-delta slew without changing static pitch. |
| `boc_amount` | Not implemented | Add a very low-rate, bounded common pitch drift only after glide passes. |
| `wobble_amount` | Active, simplified | Current deterministic per-saw detune is deliberately restrained. |
| `wobble_speed` | Active, simplified | Controls the restrained deterministic wobble rate. |
| `saw_level` | Active | Current output level is safety-limited to preserve knob-space audibility. |
| `sub_level` | Active | Current output level is safety-limited to preserve knob-space audibility. |
| `noise_level` | Deliberately disabled | Do not restore: pink noise previously caused instability and masked the oscillator. |
| `comb_depth` | Active | Retained in the stable signed-feedback comb. |
| `comb_mul` | Active | Retained as the comb pitch ratio. |

Each new parameter trial must be made in a new subfolder copied from this
modal WebUSB fallback. The fallback source and its persistence UF2 are not
modified by those trials.
