# Iris Insight

*sight & understanding*

Version 0.1.0.

Lets an AI agent watch a program for a few seconds, so it can understand and solve a problem it could not see before.

AI coding agents read code and logs well, but they cannot see what your program draws on screen. If the picture freezes, turns black, stutters or breaks, the agent has to guess, or to look at many expensive screenshots. Iris Insight gives it the last seconds of the program in one cheap picture and a short text.

It is made for any program that draws on screen and can hand over its frames as pixels. It must be compiled into the program it watches (C++17). So far it has been tested on Windows and on one program: the [Shadow of Rome PC port](https://github.com/Korvonordico/shadow-of-rome-recompiled), a game.

## What it does

While your program runs, Iris Insight keeps its last frames and log lines in memory. When something goes wrong (a crash, a freeze, a black screen, a slowdown), or when you ask, it saves a folder with:

- **a contact sheet:** one picture with up to 25 numbered frames, oldest first;
- **`summary.txt`:** a short text, about 400 tokens (cheap for an AI to read), on what pictures cannot show: when the picture last changed, frames per second, whether the logic still runs, whether the program is loading, the last inputs and errors.

You give your agent the two files instead of 25 screenshots. For example, these are the first lines of a real summary, saved while a still menu screen ran at less than half its speed. In the pictures alone, this slowdown does not show:

```text
reason: slow; saved at #4746, 99.88 s (writing the files took 0.0 s)
picture: the newest different picture is #4746 at 99.84 s; then the same picture for 0.0 s
frames shown: 18.9/s in the last second, best 51.4/s; last frame 0.0 s before the save
speed: SLOW NOW, about 21 frames/s for the last 6 s, against a best of 51/s (40%). The pictures can look normal: this line is what tells it
logic (pad reads per second): running, 50 in the last second
loading: no, no read for the last 93.8 s (the last group: 3 reads, 5.87-6.07 s)
```

## Add it to your program

You need a C++17 compiler. Built and tested only for Windows, with [llvm-mingw](https://github.com/mstorsjo/llvm-mingw).

1. Copy the three files of `src/` into your project, and add `blackbox.cpp` to your build. (The code is called "black box" because it works like the flight recorder of a plane.)

2. Start it as early as you can in `main()`:

   ```cpp
   #include "blackbox.h"

   blackbox::Config config;
   blackbox::start(config);
   ```

3. Give it every frame you show, as RGBA pixels. This is the only capture point:

   ```cpp
   blackbox::frame(pixels, width, height);
   ```

4. Before the normal end of your program, say that it is a normal end. Otherwise the exit is saved as a problem:

   ```cpp
   blackbox::cleanExit();
   ```

That is enough. Saves appear in the `blackbox/` folder, next to where the program runs. Only the last 5 are kept.

To save at a moment you choose (for example on a key), call `blackbox::save("my-name");`. To add a line to the log, call `blackbox::event("opened %s", name);`. Everything your program prints is copied into the log as well.

The summary gets better with a few more lines before `start()`:

```cpp
config.progress = myLoopCounter;            // a function returning a number that moves while the logic runs
config.progressName = "main loop turns";    // what that number counts
static const char *const loading[] = {"[load]", nullptr};
config.loadingLinesStartingWith = loading;  // log lines that mean "reading files"
```

Then ask your agent something like: *"Look at the newest folder in `blackbox/`: read `summary.txt` together with the picture and tell me what went wrong and from which frame."*

A tiny program that uses all of this is in [`examples/minimal`](examples/minimal/main.cpp). It draws into memory, then slows down and stops drawing on purpose, and it should leave its saves in a `blackbox/` folder where you start it. So far it has only been compiled, not run: all the tests below are on the real program.

```text
cd examples/minimal
x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -I../../src main.cpp ../../src/blackbox.cpp -o example.exe
./example.exe
```

A complete real use is in [`examples/shadow-of-rome`](examples/shadow-of-rome): the code that connects Iris Insight to that game, with its progress counter, scenes, inputs and disk reads. It shows the wiring and does not build on its own. All the details and options are in [NOTES.md](NOTES.md).

## Measured tests

Each try is a new, clean agent (Claude Sonnet) that gets one problem and answers what is wrong and from which frame. In the second and third test the correct answers were written before running the agents. Costs are real API costs. All of this is on one program, and the problems were chosen and graded by the author of the tool, so read the numbers as a first indication, not as proof.

**First test, 1 November 2026: 5 problems, 3 tries each.**

| | Average cost per try | Steps | Correct |
|---|---|---|---|
| Contact sheet + summary | $0.030 | 3 | 97% |
| Contact sheet only | $0.027 | 2 | 80% |
| Separate screenshots | $0.041 | 11 | 73% |

Weak point: the summaries for this test were rebuilt from older saves, and in the first run of it, a bug in the summary brought the result down to 77% (worse than the contact sheet alone) until it was fixed.

**Second test, 1 November 2026: 9 new problems, 3 tries each**, saved by the program itself. Most problems were simulated with test commands: 3 freezes, 2 slowdowns, 1 black screen, 3 cases with nothing wrong.

| | Average cost per try | Steps | Correct |
|---|---|---|---|
| Contact sheet + summary | $0.031 | 3 | 89% |
| Separate screenshots | $0.034 | 8 | 46% |

- Freezes: 9 out of 9 with the summary, 0 out of 9 with screenshots. When the picture stops, the last frames look normal, and only the summary says that no new frame arrived.
- False alarms on healthy cases: 0 with the summary, 2 with screenshots.
- Slowdown on a menu screen: bad for both (0.5 out of 3 with the summary, 1 out of 3 with screenshots).
- Black screen: found by everyone, but all 3 summary tries blamed a stuck load, because the summary said "loading" for one single disk read.
- Not tested, because they could not be produced: a broken picture and a really long load.

**Third test, 2 November 2026 (version 0.1.0): only the two cases that went badly**, after fixing the summary, with new saves.

| | Version 0.0.1 | Version 0.1.0 |
|---|---|---|
| Slowdown on a menu screen, correct | 0.5 out of 3 | 2.5 out of 3 |
| Black screen, found | 3 out of 3 | 3 out of 3 |
| Black screen, blamed on a stuck load | 3 out of 3 | 0 out of 3 |

6 tries with the contact sheet + summary, $0.19 in total, $0.032 per try. Screenshots were not tested again.

Weak points: only two cases, both simulated, 3 tries each. One try on the slowdown still said that nothing is clearly broken and called it a mild dip. The summary now writes "SLOW NOW" in plain words and the agent can copy it: that is the purpose, but the test also measures how well the summary is worded.

## Roadmap

- 31 October 2026: first version, inside the Shadow of Rome PC port: the last frames in memory, the numbered contact sheet, automatic saves on crash, freeze, black screen and slowdown.
- 1 November 2026: `summary.txt`, which tells a freeze from a long load; first two measured tests; own repository.
- 2 November 2026: version 0.1.0: clearer loading and speed lines, usable in any C++17 program, third test.
- Next: support for more programs.

## Part of F-AGI

Iris Insight works on its own: you do not need anything else. It is also one module of [F-AGI](https://github.com/Korvonordico/f-agi), a set of tools that work together around an AI agent.

- Module used to build it: [Token Tranche](https://github.com/Korvonordico/quotient-claude-tokens) (today still called Quotient), prudence & measure.
- Built with Iris Insight: [Shadow of Rome PC port](https://github.com/Korvonordico/shadow-of-rome-recompiled).

## License

[PolyForm Noncommercial 1.0.0](LICENSE): free for people and non-commercial use. Companies that want to use it commercially, please open an issue to ask for a commercial license.

Made with AI. I am not a programmer; sorry for the mistakes, and issues are welcome.
