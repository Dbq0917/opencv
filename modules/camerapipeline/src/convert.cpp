#include "precomp.hpp"

namespace cv {
namespace camerapipeline {
namespace {

std::string normalizeToken(const std::string& value)
{
    std::string result;
    result.reserve(value.size());
    for (size_t i = 0; i < value.size(); ++i)
    {
        const unsigned char ch = static_cast<unsigned char>(value[i]);
        if (std::isalnum(ch))
            result.push_back(static_cast<char>(std::tolower(ch)));
    }
    return result;
}

bool parseColorRangeToken(const std::string& token, ColorRange& range)
{
    if (token == "full" || token == "fullrange" || token == "pc" || token == "jpeg" ||
        token == "1" || token == "true" || token == "yes" || token == "on")
    {
        range = ColorRange::FULL;
        return true;
    }
    if (token == "limited" || token == "limitedrange" || token == "narrow" ||
        token == "narrowrange" || token == "tv" || token == "mpeg" ||
        token == "video" || token == "0" || token == "false" || token == "no" ||
        token == "off")
    {
        range = ColorRange::LIMITED;
        return true;
    }
    return false;
}

int parseBitDepthToken(const std::string& token, int fallback)
{
    if (token.empty())
        return fallback;

    char* end = 0;
    const long parsed = std::strtol(token.c_str(), &end, 10);
    if (end == token.c_str() || parsed < 8 || parsed > 16)
        return fallback;
    return static_cast<int>(parsed);
}

bool metadataKeyMatches(const std::string& key, const char* normalizedName)
{
    return normalizeToken(key) == normalizedName;
}

struct RangeBounds
{
    int black;
    int white;
};

RangeBounds getRangeBounds(ColorRange range, int bitDepth)
{
    CV_Assert(bitDepth >= 8 && bitDepth <= 16);

    if (range == ColorRange::FULL)
    {
        const RangeBounds bounds = { 0, (1 << bitDepth) - 1 };
        return bounds;
    }

    CV_Assert(range == ColorRange::LIMITED);
    const int scale = 1 << (bitDepth - 8);
    const RangeBounds bounds = { 16 * scale, 235 * scale };
    return bounds;
}

int resolveBitDepth(const FrameMetadata& metadata, const ConversionOptions& options)
{
    const int fallback = options.sourceBitDepth > 0 ? options.sourceBitDepth : 10;
    int bitDepth = metadata.getBitDepth(fallback);
    if (bitDepth < 8 || bitDepth > 16)
        bitDepth = 10;
    return bitDepth;
}

inline uint16_t scaleToFixed8(int value, const RangeBounds& bounds)
{
    if (value <= bounds.black)
        return 0;
    if (value >= bounds.white)
        return static_cast<uint16_t>(255 * 256);

    const double scale = 255.0 / static_cast<double>(bounds.white - bounds.black);
    const double scaled = (value - bounds.black) * scale;
    int fixed = static_cast<int>(std::floor(scaled * 256.0 + 0.5));
    if (fixed < 0)
        fixed = 0;
    if (fixed > 255 * 256)
        fixed = 255 * 256;
    return static_cast<uint16_t>(fixed);
}

std::vector<uint16_t> makeFixedPointTable(const RangeBounds& bounds)
{
    std::vector<uint16_t> table(static_cast<size_t>(bounds.white) + 1);
    for (int value = 0; value <= bounds.white; ++value)
        table[static_cast<size_t>(value)] = scaleToFixed8(value, bounds);
    return table;
}

const uchar bayer4x4[4][4] = {
    {  0,  8,  2, 10 },
    { 12,  4, 14,  6 },
    {  3, 11,  1,  9 },
    { 15,  7, 13,  5 }
};

inline int orderedThreshold(int row, int column)
{
    return static_cast<int>(bayer4x4[row & 3][column & 3]) * 16 + 8;
}

std::vector<uchar> makeOrderedTable(const RangeBounds& bounds)
{
    const size_t stride = static_cast<size_t>(bounds.white) + 1;
    std::vector<uchar> table(stride * 16);
    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            const int threshold = orderedThreshold(row, column);
            const size_t base = static_cast<size_t>(row * 4 + column) * stride;
            for (int value = 0; value <= bounds.white; ++value)
            {
                const int fixed = static_cast<int>(
                    scaleToFixed8(value, bounds));
                const int quantized = (fixed >> 8) +
                    (((fixed & 255) > threshold) ? 1 : 0);
                table[base + static_cast<size_t>(value)] =
                    static_cast<uchar>(quantized);
            }
        }
    }
    return table;
}

const std::vector<uchar>* cachedOrderedTable(ColorRange range, int bitDepth)
{
    if (bitDepth == 10)
    {
        static const std::vector<uchar> full =
            makeOrderedTable(getRangeBounds(ColorRange::FULL, 10));
        static const std::vector<uchar> limited =
            makeOrderedTable(getRangeBounds(ColorRange::LIMITED, 10));
        return range == ColorRange::FULL ? &full : &limited;
    }
    if (bitDepth == 12)
    {
        static const std::vector<uchar> full =
            makeOrderedTable(getRangeBounds(ColorRange::FULL, 12));
        static const std::vector<uchar> limited =
            makeOrderedTable(getRangeBounds(ColorRange::LIMITED, 12));
        return range == ColorRange::FULL ? &full : &limited;
    }
    return 0;
}

const std::vector<uint16_t>* cachedFixedPointTable(ColorRange range, int bitDepth)
{
    if (bitDepth == 10)
    {
        static const std::vector<uint16_t> full =
            makeFixedPointTable(getRangeBounds(ColorRange::FULL, 10));
        static const std::vector<uint16_t> limited =
            makeFixedPointTable(getRangeBounds(ColorRange::LIMITED, 10));
        return range == ColorRange::FULL ? &full : &limited;
    }
    if (bitDepth == 12)
    {
        static const std::vector<uint16_t> full =
            makeFixedPointTable(getRangeBounds(ColorRange::FULL, 12));
        static const std::vector<uint16_t> limited =
            makeFixedPointTable(getRangeBounds(ColorRange::LIMITED, 12));
        return range == ColorRange::FULL ? &full : &limited;
    }
    return 0;
}

void convertOrdered(const Mat& src, Mat& dst, const std::vector<uchar>& table,
                    int whiteLevel)
{
    const int rows = src.rows;
    const int columns = src.cols;
    const size_t stride = static_cast<size_t>(whiteLevel) + 1;

    class OrderedBody CV_FINAL : public ParallelLoopBody
    {
    public:
        OrderedBody(const Mat& source, Mat& destination,
                    const std::vector<uchar>& values, int white, size_t tableStride)
            : src_(source), dst_(destination), table_(values), white_(white),
              stride_(tableStride) {}

        void operator()(const Range& range) const CV_OVERRIDE
        {
            const int channels_ = src_.channels();
            for (int row = range.start; row < range.end; ++row)
            {
                const ushort* source = src_.ptr<ushort>(row);
                uchar* destination = dst_.ptr<uchar>(row);
                const uchar* thresholds = table_.data() + ((row & 3) * 4) * stride_;
                if (channels_ == 3)
                {
                    for (int column = 0; column < src_.cols; ++column)
                    {
                        const uchar* thresholdTable = thresholds + (column & 3) * stride_;
                        const int sourceOffset = column * 3;
                        const int value0 = source[sourceOffset];
                        const int value1 = source[sourceOffset + 1];
                        const int value2 = source[sourceOffset + 2];
                        destination[sourceOffset] = value0 <= white_
                            ? thresholdTable[static_cast<size_t>(value0)] : 255;
                        destination[sourceOffset + 1] = value1 <= white_
                            ? thresholdTable[static_cast<size_t>(value1)] : 255;
                        destination[sourceOffset + 2] = value2 <= white_
                            ? thresholdTable[static_cast<size_t>(value2)] : 255;
                    }
                    continue;
                }
                if (channels_ == 4)
                {
                    for (int column = 0; column < src_.cols; ++column)
                    {
                        const uchar* thresholdTable = thresholds + (column & 3) * stride_;
                        const int sourceOffset = column * 4;
                        const int value0 = source[sourceOffset];
                        const int value1 = source[sourceOffset + 1];
                        const int value2 = source[sourceOffset + 2];
                        const int value3 = source[sourceOffset + 3];
                        destination[sourceOffset] = value0 <= white_
                            ? thresholdTable[static_cast<size_t>(value0)] : 255;
                        destination[sourceOffset + 1] = value1 <= white_
                            ? thresholdTable[static_cast<size_t>(value1)] : 255;
                        destination[sourceOffset + 2] = value2 <= white_
                            ? thresholdTable[static_cast<size_t>(value2)] : 255;
                        destination[sourceOffset + 3] = value3 <= white_
                            ? thresholdTable[static_cast<size_t>(value3)] : 255;
                    }
                    continue;
                }
                for (int column = 0; column < src_.cols; ++column)
                {
                    const uchar* thresholdTable = thresholds + (column & 3) * stride_;
                    const int sourceOffset = column * channels_;
                    for (int channel = 0; channel < channels_; ++channel)
                    {
                        const int value = source[sourceOffset + channel];
                        destination[sourceOffset + channel] =
                            value <= white_
                                ? thresholdTable[static_cast<size_t>(value)]
                                : 255;
                    }
                }
            }
        }

    private:
        const Mat& src_;
        Mat& dst_;
        const std::vector<uchar>& table_;
        int white_;
        size_t stride_;
    };

    if (static_cast<int64_t>(rows) * columns >= 64 * 64)
        parallel_for_(Range(0, rows), OrderedBody(src, dst, table, whiteLevel, stride));
    else
        OrderedBody(src, dst, table, whiteLevel, stride)(Range(0, rows));
}

void convertFloydSteinberg(const Mat& src, Mat& dst,
                           const std::vector<uint16_t>& table, int whiteLevel)
{
    const int rows = src.rows;
    const int columns = src.cols;
    const int channels = src.channels();
    const size_t stride = static_cast<size_t>(columns + 2) * channels;

    std::vector<int32_t> current(stride, 0);
    std::vector<int32_t> next(stride, 0);

    for (int row = 0; row < rows; ++row)
    {
        const ushort* source = src.ptr<ushort>(row);
        uchar* destination = dst.ptr<uchar>(row);
        std::fill(next.begin(), next.end(), 0);

        for (int column = 0; column < columns; ++column)
        {
            const int sourceOffset = column * channels;
            const size_t errorOffset = static_cast<size_t>(column + 1) * channels;

            for (int channel = 0; channel < channels; ++channel)
            {
                const int value = source[sourceOffset + channel];
                const int fixed = value <= whiteLevel
                    ? static_cast<int>(table[static_cast<size_t>(value)])
                    : 255 * 256;
                const int32_t accumulated =
                    static_cast<int32_t>(fixed) + current[errorOffset + channel];

                int result;
                if (accumulated >= 0)
                    result = (accumulated + 128) >> 8;
                else
                    result = -(((-accumulated) + 128) >> 8);
                result = result < 0 ? 0 : (result > 255 ? 255 : result);
                destination[sourceOffset + channel] = static_cast<uchar>(result);

                const int32_t error = accumulated - result * 256;
                current[errorOffset + channel + channels] += (error * 7) / 16;
                next[errorOffset + channel - channels] += (error * 3) / 16;
                next[errorOffset + channel] += (error * 5) / 16;
                next[errorOffset + channel + channels] += error / 16;
            }
        }

        current.swap(next);
    }
}

} // namespace

FrameMetadata::FrameMetadata()
    : colorRange(ColorRange::UNKNOWN), bitDepth(0)
{
}

FrameMetadata::FrameMetadata(ColorRange range, int depth)
    : colorRange(range), bitDepth(depth)
{
}

FrameMetadata::FrameMetadata(const std::map<std::string, std::string>& values)
    : colorRange(ColorRange::UNKNOWN), bitDepth(0), properties(values)
{
}

bool FrameMetadata::parseColorRange(const std::string& value, ColorRange& range)
{
    return parseColorRangeToken(normalizeToken(value), range);
}

ColorRange FrameMetadata::getColorRange() const
{
    if (colorRange == ColorRange::FULL || colorRange == ColorRange::LIMITED)
        return colorRange;

    static const char* rangeKeys[] = {
        "colorrange",
        "videofullrangeflag",
        "fullrange",
        "yuvrange",
        "matrixrange",
        "range",
        "cicpvideofullrangeflag"
    };

    for (size_t keyIndex = 0; keyIndex < sizeof(rangeKeys) / sizeof(rangeKeys[0]); ++keyIndex)
    {
        for (std::map<std::string, std::string>::const_iterator it = properties.begin();
             it != properties.end(); ++it)
        {
            if (!metadataKeyMatches(it->first, rangeKeys[keyIndex]))
                continue;

            ColorRange parsed;
            if (parseColorRange(it->second, parsed))
                return parsed;
        }
    }

    return ColorRange::UNKNOWN;
}

int FrameMetadata::getBitDepth(int fallbackBitDepth) const
{
    if (bitDepth >= 8 && bitDepth <= 16)
        return bitDepth;

    static const char* depthKeys[] = {
        "bitdepth",
        "bitspercomponent",
        "componentbitdepth",
        "sourcebitdepth",
        "componentdepth"
    };

    for (size_t keyIndex = 0; keyIndex < sizeof(depthKeys) / sizeof(depthKeys[0]); ++keyIndex)
    {
        for (std::map<std::string, std::string>::const_iterator it = properties.begin();
             it != properties.end(); ++it)
        {
            if (!metadataKeyMatches(it->first, depthKeys[keyIndex]))
                continue;

            const int parsed = parseBitDepthToken(it->second, 0);
            if (parsed >= 8 && parsed <= 16)
                return parsed;
        }
    }

    return fallbackBitDepth;
}

void FrameMetadata::setProperty(const std::string& key, const std::string& value)
{
    properties[key] = value;
}

ConversionOptions::ConversionOptions()
    : fallbackRange(DEFAULT_COLOR_RANGE),
      ditherMode(DitherMode::AUTO),
      purpose(ConversionPurpose::ARCHIVE),
      sourceBitDepth(10)
{
}

ColorRange resolveColorRange(const FrameMetadata& metadata, ColorRange fallbackRange)
{
    const ColorRange metadataRange = metadata.getColorRange();
    if (metadataRange != ColorRange::UNKNOWN)
        return metadataRange;
    if (fallbackRange == ColorRange::FULL || fallbackRange == ColorRange::LIMITED)
        return fallbackRange;
    return DEFAULT_COLOR_RANGE;
}

DitherMode resolveDitherMode(DitherMode requested, ConversionPurpose purpose)
{
    if (purpose == ConversionPurpose::ARCHIVE)
        return DitherMode::NONE;
    if (requested != DitherMode::AUTO)
        return requested;
    return DitherMode::ORDERED;
}

void convertTo8Bit(InputArray srcArray, OutputArray dstArray,
                   const FrameMetadata& metadata, const ConversionOptions& options)
{
    const Mat src = srcArray.getMat();
    CV_Assert(!src.empty());
    CV_Assert(src.dims == 2);

    const int channels = src.channels();
    CV_Assert(channels >= 1 && channels <= CV_CN_MAX);

    if (src.depth() == CV_8U)
    {
        dstArray.create(src.size(), src.type());
        const Mat dst = dstArray.getMat();
        if (dst.data != src.data)
            src.copyTo(dst);
        return;
    }

    CV_Assert(src.depth() == CV_16U);

    const int bitDepth = resolveBitDepth(metadata, options);
    const ColorRange range = resolveColorRange(metadata, options.fallbackRange);
    const DitherMode dither = resolveDitherMode(options.ditherMode, options.purpose);
    const RangeBounds bounds = getRangeBounds(range, bitDepth);

    dstArray.create(src.size(), CV_MAKETYPE(CV_8U, channels));
    Mat dst = dstArray.getMat();
    CV_Assert(src.data == 0 || dst.data == 0 || src.data != dst.data);

    if (dither == DitherMode::NONE)
    {
        const double scale = 255.0 / static_cast<double>(bounds.white - bounds.black);
        const double shift = -bounds.black * scale;
        src.convertTo(dst, CV_8U, scale, shift);
        return;
    }

    if (dither == DitherMode::ORDERED)
    {
        const std::vector<uchar>* table = cachedOrderedTable(range, bitDepth);
        std::vector<uchar> computed;
        if (!table)
        {
            computed = makeOrderedTable(bounds);
            table = &computed;
        }
        convertOrdered(src, dst, *table, bounds.white);
    }
    else if (dither == DitherMode::FLOYD_STEINBERG)
    {
        const std::vector<uint16_t>* table = cachedFixedPointTable(range, bitDepth);
        std::vector<uint16_t> computed;
        if (!table)
        {
            computed = makeFixedPointTable(bounds);
            table = &computed;
        }
        convertFloydSteinberg(src, dst, *table, bounds.white);
    }
    else
        CV_Error(Error::StsBadArg, "Unsupported dither mode");
}

void convertTo8Bit(InputArray src, OutputArray dst,
                   const ConversionOptions& options)
{
    convertTo8Bit(src, dst, FrameMetadata(), options);
}

void convertTo8Bit(InputArray src, OutputArray dst,
                   const std::map<std::string, std::string>& metadata,
                   const ConversionOptions& options)
{
    convertTo8Bit(src, dst, FrameMetadata(metadata), options);
}

void convertTo8Bit(InputArray src, OutputArray dst, ColorRange range,
                   const ConversionOptions& options)
{
    ConversionOptions explicitOptions = options;
    explicitOptions.fallbackRange = range;
    convertTo8Bit(src, dst, FrameMetadata(), explicitOptions);
}

} // namespace camerapipeline
} // namespace cv
