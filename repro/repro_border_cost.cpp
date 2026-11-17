#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <chrono>

using namespace cv;
using clk = std::chrono::steady_clock;

// Measures the border-construction cost in isolation, alternating the
// non-isolated and isolated flags within the same process to cancel out
// system noise. The two paths do the same pixel work; the only difference
// is a handful of ROI pointer adjustments, so the timings must match.
template <int Border>
static double run(const Mat& tile, int iters)
{
    Mat bordered;
    auto t0 = clk::now();
    for (int i = 0; i < iters; ++i)
        copyMakeBorder(tile, bordered, 13, 13, 13, 13, Border);
    auto t1 = clk::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count() / iters;
}

int main()
{
    Mat parent(512, 512, CV_8UC1);
    RNG(7).fill(parent, RNG::UNIFORM, 0, 256);
    Mat tile(parent, Rect(64, 64, 256, 256));
    const int iters = 2000;

    for (int round = 0; round < 4; ++round)
    {
        double tDefault = run<BORDER_DEFAULT>(tile, iters);
        double tIsolated = run<BORDER_DEFAULT | BORDER_ISOLATED>(tile, iters);
        printf("round %d: non-isolated %7.3f ms  isolated %7.3f ms  delta %+.3f ms\n",
               round, tDefault, tIsolated, tIsolated - tDefault);
    }
    return 0;
}
