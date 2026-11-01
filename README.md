# Iris Insight

*sight & understanding*

Lets an AI agent see a running game: a cheap mosaic of the last frames plus a short log, so it can tell a freeze from a long load, spot a slowdown and point at the frame where the picture broke.

Version 0.0.1, private draft. Part of F-AGI, next to Token Tranche (resources) and Psyche Map (memory & knowledge).

## How it works, in short

The game keeps its last frames and log lines in memory. When something goes wrong (crash, freeze, black screen, slowdown, or a key you press), it saves one picture with up to 25 numbered frames and a `summary.txt` of about 400 tokens: when the picture last changed, frames per second, whether the game logic still runs, whether it is loading, the scene, the last inputs and errors. The agent reads the two together instead of 25 screenshots.

Full technical notes: [NOTES.md](NOTES.md). The code is in `src/`; `examples/shadow-of-rome/` shows how it is wired into one game (the Shadow of Rome PC port).

## Measured (1 November 2026, Claude Sonnet, 5 problems, 3 tries each)

| | Average cost | Steps | Correct |
|---|---|---|---|
| Mosaic + summary | $0.030 | 3 | 97% |
| Mosaic only | $0.027 | 2 | 80% |
| Separate screenshots | $0.041 | 11 | 73% |

The freeze on a loading screen was recognized 3 out of 3 times with the summary, 0 out of 3 without. Weak evidence: few problems, and the summaries for the test were rebuilt from older saves.

## Status

Tested only on Windows and on one game. No license yet: all rights reserved until one is chosen.

Made with AI. I am not a programmer; sorry for the mistakes, and issues are welcome.
