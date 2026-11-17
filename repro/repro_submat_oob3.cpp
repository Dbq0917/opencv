#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Service model: a tile is a submatrix of a parent whose backing heap
// allocation is only as large as the tile itself. Non-isolated border
// extension adjusts the ROI and reads parent pixels outside that
// allocation -> AddressSanitizer heap-buffer-overflow.
int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 10;
    const int tileRows = 120, tileCols = 160;

    for (int it = 0; it < iters; ++it)
    {
        const size_t bytes = (size_t)tileRows * tileCols * CV_ELEM_SIZE(CV_8UC1);
        uchar* raw = (uchar*)std::malloc(bytes);
        RNG rng(7u + it);
        for (size_t i = 0; i < bytes; ++i) raw[i] = (uchar)rng.uniform(0, 256);

        // Parent header shares the tile allocation; the ROI occupies the
        // top-left region while the parent claims an extra row/col at the
        // bottom/right that is not backed by readable memory.
        Mat parent(tileRows, tileCols, CV_8UC1, raw);
        Mat tile(parent, Rect(0, 0, tileCols - 1, tileRows - 1));
        CV_Assert(tile.isSubmatrix());

        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);
        printf("iter %d ok\n", it);
        std::free(raw);
    }
    printf("DONE\n");
    return 0;
}
