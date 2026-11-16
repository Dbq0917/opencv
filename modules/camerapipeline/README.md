# Camera Pipeline Conversion

This module converts 10-bit camera components stored in `CV_16U` words to `CV_8U`
display channels.

The range convention is read from `cv::camerapipeline::FrameMetadata`. Full-range data
maps code 0 and code 1023 to 0 and 255. Limited-range data maps code 64 and code 940 to
0 and 255. When metadata does not contain a range flag, `ConversionOptions::fallbackRange`
is used; its default is `ColorRange::LIMITED`. Resolution is never inspected.

Metadata can be supplied as the direct `colorRange` field or as string properties.
Recognised property keys include `color_range`, `video_full_range_flag`, `full_range`,
`yuv_range`, `matrix_range`, `range` and `cicp_video_full_range_flag`. Values include
`full`, `pc`, `jpeg`, `limited`, `tv`, `mpeg`, and the CICP numeric flags `1` and `0`.
Component depth can be supplied through `bit_depth`, `bits_per_component`,
`component_bit_depth` or `source_bit_depth`. Raw frames without metadata use the direct
`convertTo8Bit(src, dst, ColorRange, options)` overload.

`ConversionOptions::purpose` selects the dithering policy. Archive work always uses no
dithering and keeps the conversion deterministic and noise-free. Display work defaults to
a 4x4 ordered dither; Floyd-Steinberg error diffusion is available explicitly.

The converter accepts sub-matrices and any channel count. It writes the destination
directly and does not allocate an intermediate image. `CV_8U` input is copied unchanged,
which keeps an existing eight-bit path bit-exact.

```cpp
cv::Mat src;  // CV_16UC3 camera frame
cv::Mat dst;

cv::camerapipeline::FrameMetadata metadata;
metadata.setProperty("video_full_range_flag", "1");

cv::camerapipeline::ConversionOptions options;
options.purpose = cv::camerapipeline::ConversionPurpose::DISPLAY;

cv::camerapipeline::convertTo8Bit(src, dst, metadata, options);
```

The no-dither path writes directly through `Mat::convertTo`; the dither paths write into
the destination sample by sample. No intermediate image is created. Ordered dither tables
are cached per range and component depth, so a batch of frames does not rebuild them.
