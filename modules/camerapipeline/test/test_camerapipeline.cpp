#include "test_precomp.hpp"

#include <cmath>

namespace opencv_test
{
namespace
{

using namespace cv::camerapipeline;

struct Bounds
{
    int black;
    int white;
};

Bounds boundsFor(ColorRange range, int bitDepth)
{
    if (range == ColorRange::FULL)
    {
        const Bounds bounds = { 0, (1 << bitDepth) - 1 };
        return bounds;
    }
    const int scale = 1 << (bitDepth - 8);
    const Bounds bounds = { 16 * scale, 235 * scale };
    return bounds;
}

uchar referenceValue(int value, const Bounds& bounds)
{
    const double scaled =
        (value - bounds.black) * 255.0 / static_cast<double>(bounds.white - bounds.black);
    const double clamped = std::max(0.0, std::min(255.0, scaled));
    return cv::saturate_cast<uchar>(clamped);
}

Mat makeColorChart(int bitDepth, ColorRange range)
{
    static const int patches[24][3] = {
        { 115,  82,  68 }, { 194, 150, 130 }, {  98, 122, 157 }, {  87, 108,  67 },
        { 133, 128, 177 }, { 103, 189, 170 }, { 214, 126,  44 }, {  80,  91, 166 },
        { 193,  90,  99 }, {  94,  60, 108 }, { 157, 188,  64 }, { 224, 163,  46 },
        {  56,  61, 150 }, {  70, 148,  73 }, { 175,  54,  60 }, { 231, 199,  31 },
        { 187,  86, 149 }, {   8, 133, 161 }, { 243, 243, 242 }, { 200, 200, 200 },
        { 160, 160, 160 }, { 122, 122, 121 }, {  85,  85,  85 }, {  52,  52,  52 }
    };

    const int patchWidth = 20;
    const int patchHeight = 20;
    const int columns = 6;
    const int rows = 4;
    Mat chart(rows * patchHeight, columns * patchWidth, CV_16UC3);
    const Bounds bounds = boundsFor(range, bitDepth);

    for (int patch = 0; patch < 24; ++patch)
    {
        const int patchRow = patch / columns;
        const int patchColumn = patch % columns;
        for (int channel = 0; channel < 3; ++channel)
        {
            const int code = cvRound(bounds.black +
                patches[patch][channel] * (bounds.white - bounds.black) / 255.0);
            const Rect area(patchColumn * patchWidth, patchRow * patchHeight,
                            patchWidth, patchHeight);
            Mat channelView = chart(area);
            for (int y = 0; y < patchHeight; ++y)
            {
                ushort* row = channelView.ptr<ushort>(y);
                for (int x = 0; x < patchWidth; ++x)
                    row[x * 3 + channel] = static_cast<ushort>(code);
            }
        }
    }

    return chart;
}

Mat referenceConversion(const Mat& src, const Bounds& bounds)
{
    Mat reference(src.size(), CV_MAKETYPE(CV_8U, src.channels()));
    for (int row = 0; row < src.rows; ++row)
    {
        const ushort* source = src.ptr<ushort>(row);
        uchar* destination = reference.ptr<uchar>(row);
        for (int column = 0; column < src.cols; ++column)
        {
            for (int channel = 0; channel < src.channels(); ++channel)
            {
                const int offset = column * src.channels() + channel;
                destination[offset] = referenceValue(source[offset], bounds);
            }
        }
    }
    return reference;
}

void expectNearReference(const Mat& actual, const Mat& reference, int maximumDifference)
{
    ASSERT_EQ(actual.size(), reference.size());
    ASSERT_EQ(actual.type(), reference.type());
    for (int row = 0; row < actual.rows; ++row)
    {
        const uchar* lhs = actual.ptr<uchar>(row);
        const uchar* rhs = reference.ptr<uchar>(row);
        for (int column = 0; column < actual.cols; ++column)
        {
            for (int channel = 0; channel < actual.channels(); ++channel)
            {
                const int offset = column * actual.channels() + channel;
                const int difference = std::abs(static_cast<int>(lhs[offset]) -
                                                static_cast<int>(rhs[offset]));
                if (difference > maximumDifference)
                    FAIL() << "difference " << difference << " at (" << column << ", "
                           << row << ", " << channel << ")";
            }
        }
    }
}

double meanOf(const Mat& image)
{
    return cv::mean(image)[0];
}

} // namespace

TEST(CameraPipeline, full_and_limited_color_charts_match_reference)
{
    const int bitDepths[] = { 10, 12 };
    const ColorRange ranges[] = { ColorRange::FULL, ColorRange::LIMITED };

    for (size_t depthIndex = 0; depthIndex < 2; ++depthIndex)
    {
        for (size_t rangeIndex = 0; rangeIndex < 2; ++rangeIndex)
        {
            const int bitDepth = bitDepths[depthIndex];
            const ColorRange range = ranges[rangeIndex];
            const Bounds bounds = boundsFor(range, bitDepth);
            const Mat chart = makeColorChart(bitDepth, range);
            const Mat reference = referenceConversion(chart, bounds);

            FrameMetadata metadata(range, bitDepth);
            ConversionOptions options;
            options.ditherMode = DitherMode::NONE;

            Mat converted;
            convertTo8Bit(chart, converted, metadata, options);
            expectNearReference(converted, reference, 1);
        }
    }
}

TEST(CameraPipeline, range_endpoints_are_exact)
{
    const int bitDepth = 10;
    const int fullValues[] = { 0, 1, 511, 512, 1022, 1023, 4096, 65535 };
    const uchar fullExpected[] = { 0, 0, 127, 128, 255, 255, 255, 255 };
    const int limitedValues[] = { 0, 63, 64, 65, 502, 939, 940, 941, 1023, 65535 };
    const uchar limitedExpected[] = { 0, 0, 0, 0, 128, 255, 255, 255, 255, 255 };

    ConversionOptions options;
    options.ditherMode = DitherMode::NONE;

    for (size_t i = 0; i < sizeof(fullValues) / sizeof(fullValues[0]); ++i)
    {
        Mat src(1, 4, CV_16UC1, Scalar::all(fullValues[i]));
        Mat dst;
        convertTo8Bit(src, dst, FrameMetadata(ColorRange::FULL, bitDepth), options);
        EXPECT_EQ(dst.at<uchar>(0, 0), fullExpected[i]);
    }

    for (size_t i = 0; i < sizeof(limitedValues) / sizeof(limitedValues[0]); ++i)
    {
        Mat src(1, 4, CV_16UC1, Scalar::all(limitedValues[i]));
        Mat dst;
        convertTo8Bit(src, dst, FrameMetadata(ColorRange::LIMITED, bitDepth), options);
        EXPECT_EQ(dst.at<uchar>(0, 0), limitedExpected[i]);
    }
}

TEST(CameraPipeline, all_ten_bit_codes_match_reference)
{
    const ColorRange ranges[] = { ColorRange::FULL, ColorRange::LIMITED };
    for (size_t rangeIndex = 0; rangeIndex < 2; ++rangeIndex)
    {
        const ColorRange range = ranges[rangeIndex];
        const Bounds bounds = boundsFor(range, 10);

        Mat src(1, 1024, CV_16UC1);
        for (int code = 0; code < 1024; ++code)
            src.at<ushort>(0, code) = static_cast<ushort>(code);

        ConversionOptions options;
        options.ditherMode = DitherMode::NONE;
        Mat converted;
        convertTo8Bit(src, converted, FrameMetadata(range, 10), options);

        for (int code = 0; code < 1024; ++code)
        {
            const int difference = std::abs(
                static_cast<int>(converted.at<uchar>(0, code)) -
                static_cast<int>(referenceValue(code, bounds)));
            EXPECT_LE(difference, 1) << "code " << code << " range "
                                     << static_cast<int>(range);
        }
    }
}

TEST(CameraPipeline, metadata_is_read_and_unknown_uses_explicit_fallback)
{
    FrameMetadata metadata;
    EXPECT_EQ(metadata.getColorRange(), ColorRange::UNKNOWN);

    metadata.setProperty("video_full_range_flag", "1");
    EXPECT_EQ(metadata.getColorRange(), ColorRange::FULL);

    metadata = FrameMetadata();
    metadata.setProperty("CICP_Video_Full_Range_Flag", "0");
    EXPECT_EQ(metadata.getColorRange(), ColorRange::LIMITED);

    metadata = FrameMetadata();
    metadata.setProperty("color-range", "PC");
    EXPECT_EQ(metadata.getColorRange(), ColorRange::FULL);

    metadata = FrameMetadata();
    metadata.setProperty("color_range", "unknown");
    EXPECT_EQ(metadata.getColorRange(), ColorRange::UNKNOWN);
    EXPECT_EQ(resolveColorRange(metadata, ColorRange::FULL), ColorRange::FULL);

    metadata = FrameMetadata(ColorRange::LIMITED);
    metadata.setProperty("color_range", "full");
    EXPECT_EQ(resolveColorRange(metadata, ColorRange::FULL), ColorRange::LIMITED);

    metadata = FrameMetadata();
    EXPECT_EQ(resolveColorRange(metadata, ColorRange::UNKNOWN), DEFAULT_COLOR_RANGE);

    const Mat midGray(1, 1, CV_16UC1, Scalar::all(512));
    ConversionOptions options;
    options.ditherMode = DitherMode::NONE;
    std::map<std::string, std::string> properties;
    properties["video_full_range_flag"] = "1";
    Mat converted;
    convertTo8Bit(midGray, converted, properties, options);
    EXPECT_EQ(converted.at<uchar>(0, 0),
              referenceValue(512, boundsFor(ColorRange::FULL, 10)));
}

TEST(CameraPipeline, metadata_depth_is_read)
{
    FrameMetadata metadata;
    metadata.setProperty("bit_depth", "10-bit");
    EXPECT_EQ(metadata.getBitDepth(8), 10);

    metadata = FrameMetadata();
    metadata.setProperty("bits_per_component", "12");
    EXPECT_EQ(metadata.getBitDepth(10), 12);

    metadata = FrameMetadata();
    EXPECT_EQ(metadata.getBitDepth(10), 10);

    metadata = FrameMetadata(ColorRange::FULL, 0);
    metadata.setProperty("component_bit_depth", "not-a-number");
    EXPECT_EQ(metadata.getBitDepth(10), 10);
}

TEST(CameraPipeline, raw_metadata_uses_caller_range)
{
    Mat src(3, 5, CV_16UC1, Scalar::all(1023));
    ConversionOptions options;
    options.ditherMode = DitherMode::NONE;
    options.fallbackRange = ColorRange::FULL;

    Mat converted;
    convertTo8Bit(src, converted, FrameMetadata(), options);
    EXPECT_EQ(cv::countNonZero(converted != 255), 0);

    options.fallbackRange = ColorRange::LIMITED;
    convertTo8Bit(src, converted, FrameMetadata(), options);
    EXPECT_EQ(cv::countNonZero(converted != 255), 0);

    Mat midGray(3, 5, CV_16UC1, Scalar::all(512));
    Mat explicitFull;
    convertTo8Bit(midGray, explicitFull, ColorRange::FULL, options);
    EXPECT_EQ(explicitFull.at<uchar>(0, 0),
              referenceValue(512, boundsFor(ColorRange::FULL, 10)));

    Mat explicitLimited;
    convertTo8Bit(midGray, explicitLimited, ColorRange::LIMITED, options);
    EXPECT_EQ(explicitLimited.at<uchar>(0, 0),
              referenceValue(512, boundsFor(ColorRange::LIMITED, 10)));
}

TEST(CameraPipeline, archive_default_has_no_dither)
{
    const int code = 512;
    Mat src(16, 16, CV_16UC1, Scalar::all(code));

    ConversionOptions options;
    EXPECT_EQ(options.purpose, ConversionPurpose::ARCHIVE);
    EXPECT_EQ(options.ditherMode, DitherMode::AUTO);
    EXPECT_EQ(resolveDitherMode(options.ditherMode, options.purpose), DitherMode::NONE);

    Mat converted;
    convertTo8Bit(src, converted, FrameMetadata(ColorRange::FULL, 10), options);
    EXPECT_EQ(cv::countNonZero(converted != converted.at<uchar>(0, 0)), 0);

    ConversionOptions explicitNone;
    explicitNone.purpose = ConversionPurpose::DISPLAY;
    explicitNone.ditherMode = DitherMode::NONE;
    Mat explicitResult;
    convertTo8Bit(src, explicitResult, FrameMetadata(ColorRange::FULL, 10), explicitNone);
    EXPECT_EQ(cv::norm(converted, explicitResult, NORM_INF), 0.0);

    ConversionOptions explicitDither;
    explicitDither.purpose = ConversionPurpose::ARCHIVE;
    explicitDither.ditherMode = DitherMode::ORDERED;
    EXPECT_EQ(resolveDitherMode(explicitDither.ditherMode, explicitDither.purpose),
              DitherMode::NONE);
    Mat ditheredResult;
    convertTo8Bit(src, ditheredResult, FrameMetadata(ColorRange::FULL, 10), explicitDither);
    EXPECT_EQ(cv::norm(converted, ditheredResult, NORM_INF), 0.0);
}

TEST(CameraPipeline, display_default_uses_ordered_dither)
{
    const int code = 512;
    Mat src(64, 64, CV_16UC1, Scalar::all(code));

    ConversionOptions options;
    options.purpose = ConversionPurpose::DISPLAY;
    EXPECT_EQ(resolveDitherMode(options.ditherMode, options.purpose), DitherMode::ORDERED);

    Mat converted;
    convertTo8Bit(src, converted, FrameMetadata(ColorRange::FULL, 10), options);

    double minimum = 0.0;
    double maximum = 0.0;
    cv::minMaxLoc(converted, &minimum, &maximum);
    EXPECT_GE(maximum - minimum, 1.0);
    EXPECT_LE(maximum - minimum, 1.0);

    const double ideal = 512.0 * 255.0 / 1023.0;
    EXPECT_LT(std::abs(meanOf(converted) - ideal), 0.5);
}

TEST(CameraPipeline, floyd_steinberg_is_deterministic_and_bias_free)
{
    Mat src(64, 64, CV_16UC1);
    for (int row = 0; row < src.rows; ++row)
    {
        ushort* line = src.ptr<ushort>(row);
        for (int column = 0; column < src.cols; ++column)
            line[column] = static_cast<ushort>(column * 1023 / (src.cols - 1));
    }

    ConversionOptions options;
    options.purpose = ConversionPurpose::DISPLAY;
    options.ditherMode = DitherMode::FLOYD_STEINBERG;
    FrameMetadata metadata(ColorRange::FULL, 10);

    Mat first;
    Mat second;
    convertTo8Bit(src, first, metadata, options);
    convertTo8Bit(src, second, metadata, options);
    EXPECT_EQ(cv::norm(first, second, NORM_INF), 0.0);

    const Mat reference = referenceConversion(src, boundsFor(ColorRange::FULL, 10));
    EXPECT_LT(cv::norm(first, reference, NORM_L1) / first.total(), 1.0);
}

TEST(CameraPipeline, dither_paths_handle_submatrices)
{
    Mat src(37, 53, CV_16UC3);
    cv::randu(src, Scalar::all(0), Scalar::all(1023));
    const Rect area(7, 11, 31, 19);
    const DitherMode modes[] = { DitherMode::ORDERED, DitherMode::FLOYD_STEINBERG };

    for (size_t index = 0; index < sizeof(modes) / sizeof(modes[0]); ++index)
    {
        ConversionOptions options;
        options.purpose = ConversionPurpose::DISPLAY;
        options.ditherMode = modes[index];
        FrameMetadata metadata(ColorRange::LIMITED, 10);

        Mat standalone;
        convertTo8Bit(src(area), standalone, metadata, options);

        Mat canvas(27, 39, CV_8UC3, Scalar::all(0));
        Mat destination = canvas(Rect(3, 4, area.width, area.height));
        convertTo8Bit(src(area), destination, metadata, options);
        EXPECT_EQ(cv::norm(destination, standalone, NORM_INF), 0.0);

        EXPECT_EQ(cv::norm(canvas(Rect(0, 0, canvas.cols, 4)), NORM_INF), 0.0);
        EXPECT_EQ(cv::norm(canvas(Rect(0, 4 + area.height, canvas.cols,
                                       canvas.rows - 4 - area.height)), NORM_INF), 0.0);
    }
}

TEST(CameraPipeline, channels_and_submatrices_are_supported)
{
    const int channels[] = { 1, 3, 4 };
    for (size_t index = 0; index < sizeof(channels) / sizeof(channels[0]); ++index)
    {
        const int cn = channels[index];
        Mat src(37, 53, CV_MAKETYPE(CV_16U, cn));
        cv::randu(src, Scalar::all(0), Scalar::all(1023));

        ConversionOptions options;
        options.ditherMode = DitherMode::NONE;
        FrameMetadata metadata(ColorRange::LIMITED, 10);

        Mat whole;
        convertTo8Bit(src, whole, metadata, options);

        const Rect area(7, 11, 31, 19);
        Mat destinationCanvas(27, 39, CV_MAKETYPE(CV_8U, cn), Scalar::all(0));
        Mat destination = destinationCanvas(Rect(3, 4, area.width, area.height));
        convertTo8Bit(src(area), destination, metadata, options);

        Mat expected = whole(area);
        EXPECT_EQ(cv::norm(destination, expected, NORM_INF), 0.0);

        Mat top = destinationCanvas(Rect(0, 0, destinationCanvas.cols, 4));
        Mat bottom = destinationCanvas(Rect(0, 4 + area.height, destinationCanvas.cols,
                                            destinationCanvas.rows - 4 - area.height));
        EXPECT_EQ(cv::norm(top, NORM_INF), 0.0);
        EXPECT_EQ(cv::norm(bottom, NORM_INF), 0.0);
    }
}

TEST(CameraPipeline, existing_eight_bit_samples_are_unchanged)
{
    Mat src(31, 47, CV_8UC3);
    cv::randu(src, Scalar::all(0), Scalar::all(255));

    ConversionOptions options;
    options.purpose = ConversionPurpose::DISPLAY;
    FrameMetadata metadata(ColorRange::FULL, 10);

    Mat converted;
    convertTo8Bit(src, converted, metadata, options);
    EXPECT_EQ(cv::norm(src, converted, NORM_INF), 0.0);

    Mat inPlace = src.clone();
    convertTo8Bit(inPlace, inPlace, metadata, options);
    EXPECT_EQ(cv::norm(src, inPlace, NORM_INF), 0.0);
}

TEST(CameraPipeline, mixed_full_and_limited_frames_use_their_own_metadata)
{
    const ColorRange ranges[] = { ColorRange::FULL, ColorRange::LIMITED, ColorRange::FULL,
                                  ColorRange::LIMITED };
    for (size_t index = 0; index < sizeof(ranges) / sizeof(ranges[0]); ++index)
    {
        const ColorRange range = ranges[index];
        const Bounds bounds = boundsFor(range, 10);
        const Mat chart = makeColorChart(10, range);
        const Mat reference = referenceConversion(chart, bounds);

        FrameMetadata metadata;
        metadata.setProperty("color_range",
                             range == ColorRange::FULL ? "full" : "limited");

        ConversionOptions options;
        options.ditherMode = DitherMode::NONE;
        Mat converted;
        convertTo8Bit(chart, converted, metadata, options);
        expectNearReference(converted, reference, 1);
    }
}

} // namespace opencv_test
