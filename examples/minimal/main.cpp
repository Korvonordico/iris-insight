// The smallest program that uses Iris Insight. It has no window: it draws a moving bar into memory and
// hands each frame to Iris Insight, as a real program would after drawing. It behaves well for a moment,
// then slows down, reads files, and stops drawing, so that you can read what gets saved in blackbox/.
//
// Build (Windows, llvm-mingw), from this folder:
//   x86_64-w64-mingw32-g++ -std=c++17 -O2 -static -I../../src main.cpp ../../src/blackbox.cpp -o example.exe
#include "blackbox.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <thread>
#include <vector>

namespace
{
    const int kWidth = 640, kHeight = 480;
    std::vector<uint8_t> g_image(kWidth *kHeight * 4);
    std::atomic<uint64_t> g_loopTurns{0};
    uint64_t g_frameCount = 0;

    uint64_t loopTurns() { return g_loopTurns.load(); }

    void draw()
    {
        const int bar = static_cast<int>(g_frameCount * 4 % kWidth);
        for (int y = 0; y < kHeight; ++y)
            for (int x = 0; x < kWidth; ++x)
            {
                uint8_t *p = &g_image[(static_cast<size_t>(y) * kWidth + x) * 4];
                const bool inBar = x >= bar && x < bar + 40;
                p[0] = inBar ? 255 : static_cast<uint8_t>(x * 255 / kWidth);
                p[1] = inBar ? 255 : static_cast<uint8_t>(y * 255 / kHeight);
                p[2] = 80;
                p[3] = 255;
            }
        ++g_frameCount;
    }

    // Runs the main loop for some seconds: one frame every 20 ms, plus extra milliseconds if the program is slow.
    void runFor(double seconds, int extraMilliseconds)
    {
        const auto end = std::chrono::steady_clock::now() + std::chrono::duration<double>(seconds);
        while (std::chrono::steady_clock::now() < end)
        {
            draw();
            blackbox::frame(g_image.data(), kWidth, kHeight); // the only capture point
            ++g_loopTurns;
            std::this_thread::sleep_for(std::chrono::milliseconds(20 + extraMilliseconds));
        }
    }
}

int main()
{
    static const char *const kLoadingLines[] = {"[load]", nullptr};
    blackbox::Config config;
    config.progress = loopTurns;     // a number that moves while the logic runs
    config.progressName = "main loop turns";
    config.loadingLinesStartingWith = kLoadingLines; // lines that mean "reading files"
    blackbox::start(config);

    runFor(3.0, 0);

    blackbox::event("[load] data.bin part 1"); // real loads: several reads that continue
    runFor(0.4, 0);
    blackbox::event("[load] data.bin part 2");
    runFor(0.4, 0);
    blackbox::event("[load] data.bin part 3");
    runFor(0.4, 0);
    blackbox::save("your-name"); // any time you want a look: a key, a test command

    runFor(3.0, 0);
    runFor(8.0, 40); // a slowdown: saved by itself after 5 s (reason "slow")
    runFor(3.0, 0);

    // Stops drawing: saved by itself after 6 s (reason "freeze").
    blackbox::event("waiting for something that never comes");
    std::this_thread::sleep_for(std::chrono::seconds(8));

    runFor(1.0, 0);
    blackbox::cleanExit(); // this exit is a normal one
    return 0;
}
