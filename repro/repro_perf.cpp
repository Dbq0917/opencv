#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>
#include <chrono>

using namespace cv;
using clk = std::chrono::steady_clock;

static double benchWhole(int iters)
{
    Mat img(2048, 2048, CV_8UC1);
    RNG(1).fill(img, RNG::UNIFORM, 0, 256);
    Mat out;
    auto t0 = clk::now();
    for (int i = 0; i < iters; ++i)
        fastNlMeansDenoising(img, out, 10.0f, 7, 21);
    auto t1 = clk::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count() / iters;
}

static double benchTiled(int iters, int tileSize)
{
    Mat img(2048, 2048, CV_8UC1);
    RNG(1).fill(img, RNG::UNIFORM, 0, 256);
    Mat stitched = Mat::zeros(img.size(), img.type());
    auto t0 = clk::now();
    for (int it = 0; it < iters; ++it)
    {
        for (int r = 0; r < img.rows; r += tileSize)
            for (int c = 0; c < img.cols; c += tileSize)
            {
                Rect roi(c, r, std::min(tileSize, img.cols - c),
                         std::min(tileSize, img.rows - r));
                Mat view(img, roi);
                Mat out;
                fastNlMeansDenoising(view, out, 10.0f, 7, 21);
                out.copyTo(stitched(roi));
            }
    }
    auto t1 = clk::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count() / iters;
}

int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 5;
    setNumThreads(16);
    printf("whole image      : %8.1f ms/call\n", benchWhole(iters));
    printf("tiled 512 (views): %8.1f ms/call\n", benchTiled(iters, 512));
    printf("tiled 256 (views): %8.1f ms/call\n", benchTiled(iters, 256));
    return 0;
}
