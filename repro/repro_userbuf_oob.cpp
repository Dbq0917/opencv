#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <vector>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Model of a service tile: a heap buffer sized for exactly one tile.
// The Mat wraps the user buffer with no extra padding, so any read beyond
// the tile is a genuine heap-buffer-overflow detectable by ASan.
static Mat tileView(int rows, int cols, int type, unsigned seed)
{
    const size_t bytes = (size_t)rows * cols * CV_ELEM_SIZE(type);
    uchar* data = (uchar*)std::malloc(bytes);
    RNG rng(seed);
    for (size_t i = 0; i < bytes; ++i) data[i] = (uchar)rng.uniform(0, 256);
    Mat m(rows, cols, type, data);
    m.u = nullptr;  // user-owned; keep raw malloc alive for the short test
    return m;
}

int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 20;
    for (int it = 0; it < iters; ++it)
    {
        Mat tile = tileView(120, 160, CV_8UC1, 42u + it);
        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);
        printf("iter %d ok\n", it);
        std::free(tile.data);
    }
    printf("DONE\n");
    return 0;
}
