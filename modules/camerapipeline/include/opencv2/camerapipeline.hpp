#ifndef OPENCV_CAMERAPIPELINE_HPP
#define OPENCV_CAMERAPIPELINE_HPP

#include "opencv2/core.hpp"

#include <map>
#include <string>

/**
@defgroup camerapipeline Camera Pipeline Conversion

Conversion of high bit-depth camera components to 8-bit display channels.

The conversion layer resolves the component range from frame metadata. Full-range and
limited-range data use different black and white levels, so applying one convention to
the other clips highlights or compresses the tonal range. When metadata is unavailable,
the caller supplies an explicit fallback range. Resolution, camera model and file name
are never used to infer the range.

@{
*/

namespace cv {
namespace camerapipeline {

/** @brief Component range convention carried by a frame.

`UNKNOWN` means that no range flag was found in the metadata. It is resolved through
the caller-provided fallback before conversion.
*/
enum class ColorRange
{
    UNKNOWN = 0,
    LIMITED = 1,
    FULL = 2
};

/** @brief The convention used when metadata does not contain a range flag.

The default is the limited (studio-swing) convention used by broadcast containers.
*/
static const ColorRange DEFAULT_COLOR_RANGE = ColorRange::LIMITED;

/** @brief Dithering applied while reducing component precision to eight bits.

Archive work always uses `NONE`. For display work, `AUTO` selects a 4x4 ordered
pattern and an explicit value overrides that selection.
*/
enum class DitherMode
{
    AUTO = 0,
    NONE = 1,
    ORDERED = 2,
    FLOYD_STEINBERG = 3
};

/** @brief Intended use of the converted frame.

Archive output keeps the deterministic, noise-free conversion. Display output may use
dithering to suppress banding introduced by the 8-bit reduction.
*/
enum class ConversionPurpose
{
    ARCHIVE = 0,
    DISPLAY = 1
};

/** @brief Metadata attached to a camera frame.

The direct `colorRange` field records a range flag produced by a decoder. The
`properties` map carries container or device metadata using string keys. Common CICP,
FFmpeg, GStreamer and camera keys are recognised. A direct range value takes precedence
over the map; a map value takes precedence over the conversion fallback.
*/
class CV_EXPORTS FrameMetadata
{
public:
    FrameMetadata();
    explicit FrameMetadata(ColorRange range, int bitDepth = 0);
    explicit FrameMetadata(const std::map<std::string, std::string>& properties);

    /** @brief Parses a metadata value into a range convention.

    Accepts the common full-range and limited-range spellings, including `full`, `pc`,
    `jpeg`, `limited`, `tv`, `mpeg`, and the CICP numeric values `1` and `0`.
    @return true when the value was recognised.
    */
    static bool parseColorRange(const std::string& value, ColorRange& range);

    /** @brief Returns the direct range flag or a range parsed from `properties`.

    Returns `ColorRange::UNKNOWN` when neither source contains a usable flag.
    */
    ColorRange getColorRange() const;

    /** @brief Returns the direct component depth or a depth parsed from `properties`.

    @param fallbackBitDepth Value returned when metadata does not carry a component depth.
    */
    int getBitDepth(int fallbackBitDepth) const;

    /** @brief Stores a metadata property.

    Keys are matched case-insensitively and ignore separators such as `_` and `-`.
    */
    void setProperty(const std::string& key, const std::string& value);

    ColorRange colorRange;
    int bitDepth;
    std::map<std::string, std::string> properties;
};

/** @brief Conversion parameters.

`fallbackRange` applies only when neither `FrameMetadata::colorRange` nor its property
map contains a usable range flag. `sourceBitDepth` applies when metadata does not carry
a component depth; it defaults to 10.
*/
struct CV_EXPORTS ConversionOptions
{
    ConversionOptions();

    ColorRange fallbackRange;
    DitherMode ditherMode;
    ConversionPurpose purpose;
    int sourceBitDepth;
};

/** @brief Resolves the effective range from metadata and a caller fallback. */
CV_EXPORTS ColorRange resolveColorRange(const FrameMetadata& metadata,
                                        ColorRange fallbackRange);

/** @brief Resolves `AUTO` dithering from the conversion purpose. */
CV_EXPORTS DitherMode resolveDitherMode(DitherMode requested,
                                        ConversionPurpose purpose);

/** @brief Converts a high bit-depth component image to eight bits using an explicit mode.

The source may be a sub-matrix and may have any channel count. The source depth is
`CV_16U`; each component is expected to use the low `sourceBitDepth` bits. The output has
the same size and channel count as the source and depth `CV_8U`. `CV_8U` input is copied
unchanged, so an existing eight-bit path keeps its exact samples.

No intermediate image is allocated. The destination is written directly, including when
the source and destination are sub-matrices.

@param src Source image.
@param dst Destination image.
@param options Conversion parameters.
*/
CV_EXPORTS void convertTo8Bit(InputArray src, OutputArray dst,
                              const ConversionOptions& options = ConversionOptions());

/** @brief Converts a high bit-depth component image to eight bits using frame metadata.

The range flag is read from @p metadata. If it is absent, `options.fallbackRange` is
used. The component depth is read from @p metadata when present, otherwise
`options.sourceBitDepth` is used.

@param src Source image.
@param dst Destination image.
@param metadata Frame metadata.
@param options Conversion parameters.
*/
CV_EXPORTS void convertTo8Bit(InputArray src, OutputArray dst,
                              const FrameMetadata& metadata,
                              const ConversionOptions& options = ConversionOptions());

/** @brief Converts a high bit-depth component image to eight bits using a metadata map.

Convenience overload for decoders that expose frame properties as a string map.

@param src Source image.
@param dst Destination image.
@param metadata Frame metadata properties.
@param options Conversion parameters.
*/
CV_EXPORTS void convertTo8Bit(InputArray src, OutputArray dst,
                              const std::map<std::string, std::string>& metadata,
                              const ConversionOptions& options = ConversionOptions());

/** @brief Converts a high bit-depth component image to eight bits with an explicit range.

This overload is intended for raw frames that carry no metadata. The range is used
directly; `options.fallbackRange` is ignored.

@param src Source image.
@param dst Destination image.
@param range Component range convention of the source.
@param options Conversion parameters.
*/
CV_EXPORTS void convertTo8Bit(InputArray src, OutputArray dst, ColorRange range,
                              const ConversionOptions& options = ConversionOptions());

//! @}

} // namespace camerapipeline
} // namespace cv

#endif
