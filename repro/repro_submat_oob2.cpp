#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Service model: each tile is an independently allocated heap buffer of
// exactly tileRows*tileCols pixels. The tile is handed to denoising as a
// submatrix header with a non-zero ROI offset, so copyMakeBorder's default
// (non-isolated) path adjusts the ROI toward parent edges and samples
// neighbors that do not exist in this allocation.
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

        // Submatrix header: parent geometry is tile + 1-wide margin, but the
        // backing buffer only covers the ROI region.
        const int margin = 1;
        Mat hdr(tileRows + margin, tileCols + margin, CV_8UC1,
                raw - margin * (tileCols + margin) - margin);
        Mat tile(hdr, Rect(margin, margin, tileCols, tileRows));
        CV_Assert(tile.isSubmatrix());

        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);
        printf("iter %d ok\n", it);
        std::free(raw);
    }
    printf("DONE\n");
    return 0;
}
