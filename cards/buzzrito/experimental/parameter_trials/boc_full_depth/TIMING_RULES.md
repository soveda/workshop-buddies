# Block-Rate Timing Rules

Original Buzzrito DSP receives 64 samples per `process_buzzrito()` call at
48 kHz: 750 control updates per second. Workshop Computer calls
`ProcessSample()` once per sample: 48,000 calls per second.

Any stateful original generator copied into the Workshop renderer must advance
at its original effective rate, not once on every `ProcessSample()` call.

- Glide is calculated every 64 samples, then its `ddelta_t` is applied once per
  rendered sample.
- BOC interpolation noise receives its original `wobble_speed` once per 64
  samples in this trial. Its value is held between those updates.
- Pink noise is generated at its original 48 kHz sample rate when mixed as
  audio. It is independent from and must not advance block-rate BOC or wobble
  state.

The original `update_interp_noise_q13()` is therefore not safe to call once
per Workshop `ProcessSample()` with an unscaled original `wobble_speed`: that
would advance its motion approximately 64 times too quickly.
