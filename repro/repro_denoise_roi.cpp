#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <thread>
#include <atomic>
#include <cstdio>
#include <cstdlib>

using namespace cv;

// Standalone allocation: each block owns exactly its own heap buffer.
// A view of this block cannot legally read any neighbor pixel, so an
// out-of-region access is a real heap-buffer-overflow under ASan.
static Mat standaloneBlock(int rows, int cols, int type, unsigned seed)
{
    Mat block(rows, cols, type);
    RNG rng(seed);
    rng.fill(block, RNG::UNIFORM, 0, 256);
    return block;
}

int main(int argc, char** argv)
{
    const int iters = argc > 1 ? std::atoi(argv[1]) : 50;
    const int blockRows = 120, blockCols = 160;

    printf("== scenario 1: standalone block view (ASan heap-bounds) ==\n");
    for (int it = 0; it < iters; ++it)
    {
        Mat block = standaloneBlock(blockRows, blockCols, CV_8UC1, 1234u + it);
        Mat dst;
        fastNlMeansDenoising(block, dst, 10.0f, 7, 21);
        if (dst.size() != block.size()) { printf("size mismatch\n"); return 2; }
    }
    printf("scenario 1 done\n");

    printf("== scenario 2: tiled denoise equals whole-image denoise ==\n");
    Mat big(400, 500, CV_8UC1);
    RNG rng(99);
    rng.fill(big, RNG::UNIFORM, 0, 256);

    Mat whole;
    fastNlMeansDenoising(big, whole, 10.0f, 7, 21);

    Mat tiled = Mat::zeros(big.size(), big.type());
    const int tile = 128;
    for (int r = 0; r < big.rows; r += tile)
        for (int c = 0; c < big.cols; c += tile)
        {
            Rect roi(c, r, std::min(tile, big.cols - c), std::min(tile, big.rows - r));
            Mat view(big, roi);
            Mat out;
            fastNlMeansDenoising(view, out, 10.0f, 7, 21);
            out.copyTo(tiled(roi));
        }

    Mat diff;
    absdiff(whole, tiled, diff);
    double maxDiff = 0;
    minMaxLoc(diff, nullptr, &maxDiff);
    printf("max |whole - tiled| = %.0f\n", maxDiff);
    if (maxDiff != 0.0) { printf("MISMATCH\n"); return 3; }

    printf("== scenario 3: parallel stress on independent blocks ==\n");
    const int nthreads = 8, perThread = 20;
    std::vector<std::thread> pool;
    std::atomic<int> errors(0);
    for (int t = 0; t < nthreads; ++t)
    {
        pool.emplace_back([&, t]() {
            for (int k = 0; k < perThread; ++k)
            {
                Mat block = standaloneBlock(blockRows, blockCols, CV_8UC1,
                                            777u + t * 1000u + k);
                Mat dst;
                fastNlMeansDenoising(block, dst, 10.0f, 7, 21);
                if (dst.size() != block.size()) errors++;
            }
        });
    }
    for (auto& th : pool) th.join();
    if (errors.load() != 0) { printf("thread errors\n"); return 4; }

    printf("ALL OK\n");
    return 0;
}
