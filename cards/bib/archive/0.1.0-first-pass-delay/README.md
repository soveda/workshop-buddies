# Bib 0.1.0 first-pass-delay fallback

This directory preserves the known-good firmware immediately before the
second-pass original-delay port:

- `bib.cpp` is the complete passing source snapshot.
- `bib_workshop.uf2` is the matching flash image.
- The SHA-256 of the UF2 is
  `8e31b410729784fd8c91562f04948724215945203d70c8d4b612ce25f2fcc6e7`.

Restore this source only if the 0.2.0 delay revision fails its focused hardware
test. The copy corresponds to commit `863114c`.
