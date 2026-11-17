#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Valid, continuous parent allocation (no stride trick). A tile is a
// submatrix whose left/top offset leaves no room on those sides, while the
// border requirement exceeds the remaining parent size on the right.
// Non-isolated border handling adjusts the ROI toward the parent's right
// edge and reads past the heap block; isolated handling never leaves the
// tile and stays fully in bounds.
int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 20;
    const int parentRows = 128, parentCols = 128;
    const int tileW = 96, tileH = 128;

    for (int it = 0; it < iters; ++it)
    {
        Mat parent(parentRows, parentCols, CV_8UC1);
        RNG rng(101u + it);
        rng.fill(parent, RNG::UNIFORM, 0, 256);

        Mat tile(parent, Rect(0, 0, tileW, tileH));  // flush left/top
        CV_Assert(tile.isSubmatrix());

        Mat dst;
        fastNlMeansDenoising(tile, dst, 10.0f, 7, 21);  // border = 13
        printf("iter %d ok\n", it);
    }
    printf("DONE\n");
    return 0;
}
