# Black box

A flight recorder for games ported to PC. It keeps the last images and log lines in RAM. When something goes wrong, it saves them as contact sheets: one picture with up to 25 numbered frames, plus a text log using the same numbers.

You can read one sheet and see what happened, instead of opening 25 screenshots. That also makes it cheap for an AI assistant to look at a problem.

It knows nothing about the game. It needs only C++17 and this folder. Windows is the only tested platform.

## What it saves, and when

| Reason | When | Images |
|---|---|---|
| `crash` | An unhandled exception, including a stack overflow | 100 |
| `abort`, `terminate`, `exit` | The program is closing on an error | 100 |
| `freeze` | No new frame, or the progress counter stopped, for 6 s | 50, then 75 at 18 s, 100 at 60 s |
| `black` | A black screen for 15 s (60 s if nothing has been shown yet) | 25, then 50 at 30 s, 100 at 60 s |
| `slow` | Frames per second under 80% of the best rate for 5 s, at most once every 5 minutes | 25 |
| any name you pass to `save()` | A key or a test command | 25, or what you ask |

Each save gets its own folder, `blackbox/2026-11-01_00-12-58_freeze/`. Only the last 5 folders are kept; a `slow` one goes before the others.

- `summary.txt`: read this with the sheet. About 400 tokens on what images alone cannot show: when the picture last changed, frames per second, whether the game logic still runs (the progress counter), whether the game is loading and for how long, the scene, the last inputs, the last errors, and each tile's frame and time. It tells a long load from a freeze and shows slowdowns. Times stop where the save began, because no image is stored while files are written.
- `frames_0000701-0000977.jpg`: a sheet, oldest image first. Each cell shows its frame number (top left) and its time in seconds since the start (bottom left).
- `#829x67` in a cell means that image stayed the same for 67 samples. Identical images and plain black ones are stored once, so a long black screen does not push out what came before it.
- `log.txt`: the last lines of the program's output and your events, each with its frame number. The first number on a line is the frame, the second is seconds since the start.
- `info.txt`: the reason, the sheets and, after a crash, the exception and the stack.
- `state.txt`: your `report` callback, if you set one. Not written after a crash.

Sheets are 1564 px wide at most, which vision models read without shrinking.

## How to connect it

```cpp
#include "blackbox/blackbox.h"

// As early as you can in main():
blackbox::Config config;
config.progress = myCounter;   // optional: a number that moves while the game's logic runs
blackbox::start(config);

// With every frame shown (RGBA, width, height). This is the only capture point:
blackbox::frame(pixels, width, height);

// Anything worth remembering:
blackbox::event("loading %s", name);

// Your own key or command:
blackbox::save("key");

// When the player closes the window, so that it is not taken for an abnormal exit:
blackbox::cleanExit();
```

Add `blackbox.cpp` to your build. `Config` in `blackbox.h` lists the thresholds and the other callbacks. For a better `summary.txt`, also set `loadingLinesStartingWith` (your file-loading log lines), `scene` (a few words on where the game is) and `input` (the buttons held now).

It also copies everything your program prints to stdout and stderr into the log. The lines still reach where they went before. Turn this off with `captureOutput = false`.

To turn the black box off, do not call `start()`: `frame()` and `event()` then return at once.

## Memory and speed

200 images of 312 x 234 take 42 MB, plus 5 MB for building a sheet. Storing an image is a short loop on the thread that calls `frame()`. Saving runs on its own thread at low priority.

## What can go wrong

- Moving the window with the mouse can stop the frames, and that can look like a freeze.
- Fades and loading screens: a black screen shorter than the threshold is ignored.
- If your game does not move the progress counter during videos or loading, raise `freezeSeconds` or leave `progress` unset.
- A crash can only be saved if the process can still run its threads. Killing it from the Task Manager saves nothing.

## Third-party code

`stb_image_write.h` by Sean Barrett (public domain or MIT, your choice), used to write the JPEG files.
