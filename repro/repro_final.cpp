#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Zero-copy tile model: the caller owns a compact heap buffer holding
// exactly tileRows x tileCols pixels, and hands the library a submatrix
// header whose parent geometry is one row larger than the buffer.
//
// Non-isolated border handling treats parent pixels outside the tile as
// readable border material and walks into the unallocated phantom row.
// Isolated handling reflects only within the tile, so the buffer end is
// never crossed.
int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 30;
    const int tileRows = 120, tileCols = 160;

    for (int it = 0; it < iters; ++it)
    {
        const size_t bytes = (size_t)tileRows * tileCols;
        uchar* buffer = (uchar*)std::malloc(bytes);
        RNG rng(2026u + it);
        for (size_t k = 0; k < bytes; ++k) buffer[k] = (uchar)rng.uniform(0, 256);

        // Parent step matches the compact buffer; one extra row is declared
        // without backing storage. Tile occupies the fully backed region.
        Mat parent(tileRows + 1, tileCols, CV_8UC1, buffer, Mat::AUTO_STEP);
        Mat tile(parent, Rect(0, 0, tileCols, tileRows));
        CV_Assert(tile.isSubmatrix());

        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);
        printf("iter %d ok\n", it);
        std::free(buffer);
    }
    printf("DONE\n");
    return 0;
}
