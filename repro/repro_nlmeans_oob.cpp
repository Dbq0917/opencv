#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Service model: a tile is a submatrix of a tightly allocated parent.
// The tile sits flush with the parent's top/left edges while parent rows
// below the tile are not padded, so non-isolated border extension adjusts
// the ROI downward/rightward and reads past the heap allocation.
int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 20;
    const int allocRows = 128, allocCols = 128;

    for (int it = 0; it < iters; ++it)
    {
        Mat parent(allocRows, allocCols, CV_8UC1);
        RNG rng(101u + it);
        rng.fill(parent, RNG::UNIFORM, 0, 256);

        const int margin = 8;
        Mat tile(parent, Rect(0, 0, allocCols - margin, allocRows));

        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);
        printf("iter %d ok\n", it);
    }
    printf("DONE\n");
    return 0;
}
