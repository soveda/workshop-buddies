# Next Staged Feature: Delay Multitap

Only proceed after the shimmer test passes without stability regressions.

Port Bibesque's proven Workshop multitap model into this original-Bib block
engine: Switch Down presses in the Delay page form a one-second phrase, adding
up to eight equal-level relative delay heads. The latest tap sets total delay
time; a deliberate X movement clears the phrase and restores one even repeat.

Write the heads into original Bib's `num_delay_taps`, `delay_tap_times_q12`,
and `delay_tap_levels_q12` globals. Preserve the current `2490` delay-feedback
cap and 64-frame timing model. Do not add pressure weighting in this stage;
the Workshop switch has no pressure data.
