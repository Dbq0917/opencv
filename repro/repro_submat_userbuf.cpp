#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Service model: a tile descriptor is a submatrix (offset > 0) of a parent
// whose backing allocation only covers the tile itself. Non-isolated border
// extension adjusts the ROI toward the parent edges and reads outside the
// allocation -> heap-buffer-overflow under AddressSanitizer.
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

        // Parent claims a larger (offset + tile) geometry while the
        // allocation only holds the tile region starting at the offset.
        const int ofs = 16;
        Mat declaredParent(tileRows + ofs, tileCols + ofs, CV_8UC1);
        Mat parent(declaredParent.rows, declaredParent.cols, CV_8UC1,
                   raw - (size_t)ofs * (tileCols + ofs));
        Mat tile(parent, Rect(ofs, ofs, tileCols, tileRows));

        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);
        printf("iter %d ok\n", it);
        std::free(raw);
    }
    printf("DONE\n");
    return 0;
}
