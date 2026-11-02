# Changelog

## 0.1.0 - 2 November 2026

- `summary.txt` now says "loading" only for real, continuing reads (at least 3 lines over at least 0.5 s, settable). Before, one disk read during a black screen was reported as loading, and agents blamed a stuck load.
- New `speed:` line in `summary.txt`: says in words if the frame rate is low now, or if it dropped earlier, compared with the best rate seen. Before, a slowdown on a still screen was easy to miss.
- Usable in any program: the code in `src/` and its texts no longer speak of one kind of program, and the summary's header and "logic" line are generic.
- New tiny example in `examples/minimal` (compiled, not run yet) and a short guide in the README.
- Fixed a missing `<cmath>` include that stopped `src/` from compiling outside its first project.
- Shadow of Rome example: disk reads now also log a line every 0.25 s inside a long read.
- Third measured test, on the two cases that went badly before.

## 0.0.1 - 1 November 2026

- First version in its own repository: frame contact sheets and `summary.txt`, automatic saves on crash, freeze, black screen and slowdown, with Shadow of Rome as an example.
- License: PolyForm Noncommercial 1.0.0.
