#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Deterministic model of a zero-copy tile backed by an exactly-sized heap
// buffer. The parent header declares more columns than each allocation row
// holds (external buffers may have a tighter stride than the header claims).
// Non-isolated border handling adjusts the tile ROI toward the parent's
// right edge and copies those phantom columns -> heap-buffer-overflow.
// With BORDER_ISOLATED the tile is treated as self-contained and no parent
// pixel outside the tile is ever touched.
int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 20;
    const int rows = 128, realCols = 128, phantomCols = 8;

    for (int it = 0; it < iters; ++it)
    {
        uchar* raw = (uchar*)std::malloc((size_t)rows * realCols);
        RNG rng(101u + it);
        for (int i = 0; i < rows * realCols; ++i)
            raw[i] = (uchar)rng.uniform(0, 256);

        Mat parent(rows, realCols + phantomCols, CV_8UC1, raw);  // tight buffer
        Mat tile(parent, Rect(0, 0, realCols, rows));            // flush-left ROI
        CV_Assert(tile.isSubmatrix());

        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);
        printf("iter %d ok\n", it);
        std::free(raw);
    }
    printf("DONE\n");
    return 0;
}
