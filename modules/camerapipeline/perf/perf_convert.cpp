#include "perf_precomp.hpp"

namespace opencv_test
{
namespace
{

using namespace cv::camerapipeline;

void baselineConvert(const Mat& src, Mat& dst, ColorRange range, int bitDepth)
{
    const int scale = 1 << (bitDepth - 8);
    const int black = range == ColorRange::FULL ? 0 : 16 * scale;
    const int white = range == ColorRange::FULL ? (1 << bitDepth) - 1 : 235 * scale;
    const double alpha = 255.0 / static_cast<double>(white - black);
    src.convertTo(dst, CV_8U, alpha, -black * alpha);
}

void prepare(Mat& src)
{
    src.create(1080, 1920, CV_16UC3);
    for (int row = 0; row < src.rows; ++row)
    {
        ushort* line = src.ptr<ushort>(row);
        for (int column = 0; column < src.cols * src.channels(); ++column)
            line[column] = static_cast<ushort>((column * 1023) / (src.cols * src.channels() - 1));
    }
}

PERF_TEST(CameraPipelineBaseline, full_none)
{
    Mat src;
    prepare(src);
    Mat dst(src.size(), CV_8UC3);
    declare.in(src).out(dst);

    TEST_CYCLE() baselineConvert(src, dst, ColorRange::FULL, 10);

    SANITY_CHECK_NOTHING();
}

PERF_TEST(CameraPipelineBaseline, limited_none)
{
    Mat src;
    prepare(src);
    Mat dst(src.size(), CV_8UC3);
    declare.in(src).out(dst);

    TEST_CYCLE() baselineConvert(src, dst, ColorRange::LIMITED, 10);

    SANITY_CHECK_NOTHING();
}

PERF_TEST(CameraPipeline, full_none)
{
    Mat src;
    prepare(src);
    Mat dst(src.size(), CV_8UC3);
    declare.in(src).out(dst);

    ConversionOptions options;
    options.ditherMode = DitherMode::NONE;
    FrameMetadata metadata(ColorRange::FULL, 10);
    TEST_CYCLE() convertTo8Bit(src, dst, metadata, options);

    SANITY_CHECK_NOTHING();
}

PERF_TEST(CameraPipeline, limited_none)
{
    Mat src;
    prepare(src);
    Mat dst(src.size(), CV_8UC3);
    declare.in(src).out(dst);

    ConversionOptions options;
    options.ditherMode = DitherMode::NONE;
    FrameMetadata metadata(ColorRange::LIMITED, 10);
    TEST_CYCLE() convertTo8Bit(src, dst, metadata, options);

    SANITY_CHECK_NOTHING();
}

PERF_TEST(CameraPipeline, full_ordered)
{
    Mat src;
    prepare(src);
    Mat dst(src.size(), CV_8UC3);
    declare.in(src).out(dst);

    ConversionOptions options;
    options.purpose = ConversionPurpose::DISPLAY;
    options.ditherMode = DitherMode::ORDERED;
    FrameMetadata metadata(ColorRange::FULL, 10);
    TEST_CYCLE() convertTo8Bit(src, dst, metadata, options);

    SANITY_CHECK_NOTHING();
}

PERF_TEST(CameraPipeline, limited_ordered)
{
    Mat src;
    prepare(src);
    Mat dst(src.size(), CV_8UC3);
    declare.in(src).out(dst);

    ConversionOptions options;
    options.purpose = ConversionPurpose::DISPLAY;
    options.ditherMode = DitherMode::ORDERED;
    FrameMetadata metadata(ColorRange::LIMITED, 10);
    TEST_CYCLE() convertTo8Bit(src, dst, metadata, options);

    SANITY_CHECK_NOTHING();
}

} // namespace
} // namespace opencv_test
