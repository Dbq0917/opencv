/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "test_precomp.hpp"

#include <cstring>

namespace opencv_test { namespace {

//#define DUMP_RESULTS

#ifdef DUMP_RESULTS
#  define DUMP(image, path) imwrite(path, image)
#else
#  define DUMP(image, path)
#endif


TEST(Photo_DenoisingGrayscale, regression)
{
    string folder = string(cvtest::TS::ptr()->get_data_path()) + "denoising/";
    string original_path = folder + "lena_noised_gaussian_sigma=10.png";
    string expected_path = folder + "lena_noised_denoised_grayscale_tw=7_sw=21_h=10.png";

    Mat original = imread(original_path, IMREAD_GRAYSCALE);
    Mat expected = imread(expected_path, IMREAD_GRAYSCALE);

    ASSERT_FALSE(original.empty()) << "Could not load input image " << original_path;
    ASSERT_FALSE(expected.empty()) << "Could not load reference image " << expected_path;

    Mat result;
    fastNlMeansDenoising(original, result, 10);

    DUMP(result, expected_path + ".res.png");

    ASSERT_EQ(0, cvtest::norm(result, expected, NORM_L2));
}

TEST(Photo_DenoisingColored, regression)
{
    string folder = string(cvtest::TS::ptr()->get_data_path()) + "denoising/";
    string original_path = folder + "lena_noised_gaussian_sigma=10.png";
    string expected_path = folder + "lena_noised_denoised_lab12_tw=7_sw=21_h=10_h2=10.png";

    Mat original = imread(original_path, IMREAD_COLOR);
    Mat expected = imread(expected_path, IMREAD_COLOR);

    ASSERT_FALSE(original.empty()) << "Could not load input image " << original_path;
    ASSERT_FALSE(expected.empty()) << "Could not load reference image " << expected_path;

    Mat result;
    fastNlMeansDenoisingColored(original, result, 10, 10);

    DUMP(result, expected_path + ".res.png");

    ASSERT_EQ(0, cvtest::norm(result, expected, NORM_L2));
}

TEST(Photo_DenoisingGrayscaleMulti, regression)
{
    const int imgs_count = 3;
    string folder = string(cvtest::TS::ptr()->get_data_path()) + "denoising/";

    string expected_path = folder + "lena_noised_denoised_multi_tw=7_sw=21_h=15.png";
    Mat expected = imread(expected_path, IMREAD_GRAYSCALE);
    ASSERT_FALSE(expected.empty()) << "Could not load reference image " << expected_path;

    vector<Mat> original(imgs_count);
    for (int i = 0; i < imgs_count; i++)
    {
        string original_path = format("%slena_noised_gaussian_sigma=20_multi_%d.png", folder.c_str(), i);
        original[i] = imread(original_path, IMREAD_GRAYSCALE);
        ASSERT_FALSE(original[i].empty()) << "Could not load input image " << original_path;
    }

    Mat result;
    fastNlMeansDenoisingMulti(original, result, imgs_count / 2, imgs_count, 15);

    DUMP(result, expected_path + ".res.png");

    ASSERT_EQ(0, cvtest::norm(result, expected, NORM_L2));
}

TEST(Photo_DenoisingColoredMulti, regression)
{
    const int imgs_count = 3;
    string folder = string(cvtest::TS::ptr()->get_data_path()) + "denoising/";

    string expected_path = folder + "lena_noised_denoised_multi_lab12_tw=7_sw=21_h=10_h2=15.png";
    Mat expected = imread(expected_path, IMREAD_COLOR);
    ASSERT_FALSE(expected.empty()) << "Could not load reference image " << expected_path;

    vector<Mat> original(imgs_count);
    for (int i = 0; i < imgs_count; i++)
    {
        string original_path = format("%slena_noised_gaussian_sigma=20_multi_%d.png", folder.c_str(), i);
        original[i] = imread(original_path, IMREAD_COLOR);
        ASSERT_FALSE(original[i].empty()) << "Could not load input image " << original_path;
    }

    Mat result;
    fastNlMeansDenoisingColoredMulti(original, result, imgs_count / 2, imgs_count, 10, 15);

    DUMP(result, expected_path + ".res.png");

    ASSERT_EQ(0, cvtest::norm(result, expected, NORM_L2));
}

TEST(Photo_White, issue_2646)
{
    cv::Mat img(50, 50, CV_8UC1, cv::Scalar::all(255));
    cv::Mat filtered;
    cv::fastNlMeansDenoising(img, filtered);

    int nonWhitePixelsCount = (int)img.total() - cv::countNonZero(filtered == img);

    ASSERT_EQ(0, nonWhitePixelsCount);
}

// Related issue : https://github.com/opencv/opencv/issues/26582
TEST(Photo_DenoisingGrayscaleMulti16bitL1, regression)
{
    const int imgs_count = 3;
    string folder = string(cvtest::TS::ptr()->get_data_path()) + "denoising/";

    vector<Mat> original_8u(imgs_count);
    vector<Mat> original_16u(imgs_count);
    for (int i = 0; i < imgs_count; i++)
    {
        string original_path = format("%slena_noised_gaussian_sigma=20_multi_%d.png", folder.c_str(), i);
        original_8u[i] = imread(original_path, IMREAD_GRAYSCALE);
        ASSERT_FALSE(original_8u[i].empty()) << "Could not load input image " << original_path;
        original_8u[i].convertTo(original_16u[i], CV_16U);
    }

    Mat result_8u, result_16u;
    std::vector<float> h = {15};
    fastNlMeansDenoisingMulti(original_8u, result_8u, /*imgToDenoiseIndex*/ imgs_count / 2, /*temporalWindowSize*/ imgs_count, h, 7, 21, NORM_L1);
    fastNlMeansDenoisingMulti(original_16u, result_16u, /*imgToDenoiseIndex*/ imgs_count / 2, /*temporalWindowSize*/ imgs_count, h, 7, 21, NORM_L1);
    DUMP(result_8u, "8u.res.png");
    DUMP(result_16u, "16u.res.png");

    cv::Mat expected;
    result_8u.convertTo(expected, CV_16U);

    EXPECT_MAT_NEAR(result_16u, expected, 1);
}

// A tile is a view of a larger image. Denoising each tile independently must
// only read pixels inside the tile and must produce exactly the same pixels as
// denoising the whole image and then cropping. See security review:
// non-isolated border extension used to read parent pixels outside the tile.
static void tiledDenoisingEqualityCheck(int depth, int cn, int normType)
{
    const int type = CV_MAKE_TYPE(depth, cn);
    const int templateWindowSize = 7;
    const int searchWindowSize = 21;
    const float h = 10.0f;

    Mat image(333, 417, type);
    randu(image, Scalar::all(0), Scalar::all(256));

    Mat whole;
    fastNlMeansDenoising(image, whole, std::vector<float>(cn, h),
                         templateWindowSize, searchWindowSize, normType);

    Mat stitched = Mat::zeros(image.size(), type);
    const int tileSize = 128;
    for (int row = 0; row < image.rows; row += tileSize)
    {
        for (int col = 0; col < image.cols; col += tileSize)
        {
            Rect roi(col, row,
                     std::min(tileSize, image.cols - col),
                     std::min(tileSize, image.rows - row));
            Mat tile(image, roi);

            // Independent, exactly-sized allocation: any read outside the
            // tile crosses the heap block and is caught by sanitizers.
            Mat compactTile;
            tile.copyTo(compactTile);
            ASSERT_TRUE(compactTile.isContinuous());

            Mat tileDenoised;
            fastNlMeansDenoising(compactTile, tileDenoised, std::vector<float>(cn, h),
                                 templateWindowSize, searchWindowSize, normType);

            tileDenoised.copyTo(stitched(roi));
        }
    }

    EXPECT_EQ(0, cvtest::norm(whole, stitched, NORM_INF));
}

TEST(Photo_DenoisingTiled, equality_with_whole_image_8U_L2)
{
    tiledDenoisingEqualityCheck(CV_8U, 1, NORM_L2);
    tiledDenoisingEqualityCheck(CV_8U, 3, NORM_L2);
}

TEST(Photo_DenoisingTiled, equality_with_whole_image_8U_L1)
{
    tiledDenoisingEqualityCheck(CV_8U, 1, NORM_L1);
    tiledDenoisingEqualityCheck(CV_8U, 2, NORM_L1);
    tiledDenoisingEqualityCheck(CV_8U, 4, NORM_L1);
}

TEST(Photo_DenoisingTiled, equality_with_whole_image_16U_L1)
{
    tiledDenoisingEqualityCheck(CV_16U, 1, NORM_L1);
    tiledDenoisingEqualityCheck(CV_16U, 3, NORM_L1);
}

// Concurrent stress: many threads denoise independent tile-sized buffers at
// once. Bounds errors are intermittent under concurrency, so this runs under
// the sanitizer-enabled CI job rather than relying on a single-thread run.
TEST(Photo_DenoisingTiled, parallel_independent_tiles_no_cross_access)
{
    const int threadCount = 8;
    const int iterationsPerThread = 25;
    const int tileRows = 137, tileCols = 159;

    parallel_for_(Range(0, threadCount), [&](const Range& range) {
        for (int thread = range.start; thread < range.end; ++thread)
        {
            for (int iteration = 0; iteration < iterationsPerThread; ++iteration)
            {
                Mat tile(tileRows, tileCols, CV_8UC1);
                RNG(thread * 1000u + iteration).fill(tile, RNG::UNIFORM, 0, 256);

                Mat denoised;
                fastNlMeansDenoising(tile, denoised, 10.0f, 7, 21);
                ASSERT_EQ(tile.size(), denoised.size());
                ASSERT_EQ(tile.type(), denoised.type());
            }
        }
    });
}

// Security regression for out-of-region reads on submatrix views.
//
// The tile is a submatrix of a parent whose backing heap buffer is one row
// shorter than the parent header claims, modelling a zero-copy tile with no
// readable neighbor storage. A non-isolated border implementation adjusts
// the tile ROI toward parent edges and dereferences that missing row; under
// AddressSanitizer this is a deterministic heap-buffer-overflow. The fixed
// implementation reflects border pixels only inside the tile.
TEST(Photo_DenoisingTiled, submatrix_view_never_reads_outside_region)
{
    const int tileRows = 120, tileCols = 160;
    const size_t backingBytes = (size_t)tileRows * tileCols * CV_ELEM_SIZE(CV_8UC1);

    for (int iteration = 0; iteration < 10; ++iteration)
    {
        cv::AutoBuffer<uchar> backing(backingBytes);
        Mat randomTile(tileRows, tileCols, CV_8UC1);
        RNG(31337u + iteration).fill(randomTile, RNG::UNIFORM, 0, 256);
        std::memcpy(backing.data(), randomTile.data, backingBytes);

        // Parent claims one extra row that the allocation does not provide.
        Mat parent(tileRows + 1, tileCols, CV_8UC1, backing.data(), Mat::AUTO_STEP);
        Mat tile(parent, Rect(0, 0, tileCols, tileRows));
        ASSERT_TRUE(tile.isSubmatrix());

        Mat denoised;
        fastNlMeansDenoising(tile, denoised, 10.0f, 7, 21);
        EXPECT_EQ(tile.size(), denoised.size());
    }
}

// Temporal multi-image denoising must likewise stay inside every input view.
TEST(Photo_DenoisingTiledMulti, equality_with_whole_image)
{
    const int frameCount = 3;
    const int type = CV_8UC1;
    std::vector<Mat> frames(frameCount);
    for (int f = 0; f < frameCount; ++f)
    {
        frames[f].create(201, 233, type);
        randu(frames[f], Scalar::all(0), Scalar::all(256));
    }

    Mat whole;
    fastNlMeansDenoisingMulti(frames, whole, frameCount / 2, frameCount, 15.0f, 7, 21);

    const int tileSize = 96;
    Mat stitched = Mat::zeros(frames[0].size(), type);
    for (int row = 0; row < frames[0].rows; row += tileSize)
    {
        for (int col = 0; col < frames[0].cols; col += tileSize)
        {
            Rect roi(col, row,
                     std::min(tileSize, frames[0].cols - col),
                     std::min(tileSize, frames[0].rows - row));

            std::vector<Mat> tileFrames(frameCount);
            for (int f = 0; f < frameCount; ++f)
                frames[f](roi).copyTo(tileFrames[f]);

            Mat tileDenoised;
            fastNlMeansDenoisingMulti(tileFrames, tileDenoised, frameCount / 2,
                                      frameCount, 15.0f, 7, 21);
            tileDenoised.copyTo(stitched(roi));
        }
    }

    EXPECT_EQ(0, cvtest::norm(whole, stitched, NORM_INF));
}

// The colored entry point splits channels and denoises the L and a/b planes
// separately; each plane must obey the same tile isolation guarantees.
TEST(Photo_DenoisingTiledColored, equality_with_whole_image)
{
    Mat image(271, 309, CV_8UC3);
    randu(image, Scalar::all(0), Scalar::all(256));

    Mat whole;
    fastNlMeansDenoisingColored(image, whole, 10.0f, 10.0f, 7, 21);

    Mat stitched = Mat::zeros(image.size(), image.type());
    const int tileSize = 110;
    for (int row = 0; row < image.rows; row += tileSize)
    {
        for (int col = 0; col < image.cols; col += tileSize)
        {
            Rect roi(col, row,
                     std::min(tileSize, image.cols - col),
                     std::min(tileSize, image.rows - row));

            Mat compactTile;
            image(roi).copyTo(compactTile);
            Mat tileDenoised;
            fastNlMeansDenoisingColored(compactTile, tileDenoised, 10.0f, 10.0f, 7, 21);
            tileDenoised.copyTo(stitched(roi));
        }
    }

    EXPECT_EQ(0, cvtest::norm(whole, stitched, NORM_INF));
}

}} // namespace opencv_test
