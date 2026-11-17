#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Multi-frame temporal denoising on compact-backed submatrix views.
int main()
{
    const int frames = 3, tileRows = 100, tileCols = 120;
    std::vector<Mat> tileViews;
    std::vector<uchar*> buffers;

    for (int f = 0; f < frames; ++f)
    {
        uchar* buffer = (uchar*)std::malloc((size_t)tileRows * tileCols);
        buffers.push_back(buffer);
        Mat parent(tileRows + 1, tileCols, CV_8UC1, buffer, Mat::AUTO_STEP);
        RNG(900u + f).fill(parent(Rect(0, 0, tileCols, tileRows)), RNG::UNIFORM, 0, 256);
        tileViews.emplace_back(parent, Rect(0, 0, tileCols, tileRows));
    }

    Mat dst;
    fastNlMeansDenoisingMulti(tileViews, dst, 1, 3, 15.0f, 7, 21);
    printf("multi dst size %d x %d\n", dst.rows, dst.cols);

    for (uchar* p : buffers) std::free(p);
    printf("DONE\n");
    return 0;
}
