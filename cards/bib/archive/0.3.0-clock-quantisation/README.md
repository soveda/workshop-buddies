# Bib 0.3.0 clock-quantisation archive

This snapshot contains the first direct port of Bib's iterative clock grid.
It is retained for history, but had a Workshop-only regression: after a Z tap,
continuous clock input remained suppressed. The subsequent revision corrects
that to match original Bib behaviour.

- Source snapshot: `bib.cpp`
- Flash image: `bib_workshop.uf2`
- UF2 SHA-256:
  `3d157784585c4a83b2c1fecaa874de80f7dba2b58f0033ad150ab964066981c7`
- Corresponding Git commit: `764b26b`
