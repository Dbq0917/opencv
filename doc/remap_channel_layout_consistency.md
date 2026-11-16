# Remap channel-layout consistency validation

## Problem

`cv::remap(..., INTER_LINEAR, ...)` used two different paths for floating-point maps:

* `8UC1`, `8UC3`, `8UC4` (and the same layouts for `16U` / `32F`) used the
  table-free kernels, which interpolate directly from the floating-point
  coordinates.
* `8UC2` and channel counts greater than four fell back to the legacy
  fixed-point table path, which first quantizes coordinates to 1/32 pixel and
  then uses 15-bit fixed-point interpolation weights.

The two paths therefore produced different values for the same image content
and the same maps. On a monotonic gradient the difference was typically one
gray level; on locally steep gradients it reached five gray levels. That is
enough to produce visible seams when registration stages use different channel
layouts.

## Fix

* Added table-free `8UC2`, `16UC2` and `32FC2` linear kernels using the same
  coordinate computation, interpolation formula, rounding and border handling
  as the existing `8UC1/8UC3/8UC4` kernels.
* Added an exact floating-coordinate bilinear fallback for channel counts
  greater than four. It uses the same `floor`, fractional weights and border
  rules as the table-free kernels.
* The integer-map path (`CV_16SC2` / `CV_16UC1`) is unchanged.
* No other interpolation mode (nearest, cubic, Lanczos) was changed.

## Per-pixel validation

Test: `Imgproc_Remap.channel_layout_consistency`

The test uses deterministic floating-point maps, a monotonic gradient image,
and the following cross product:

* depths: `8U`, `16U`, `32F`
* channels: `1, 2, 3, 4, 5, 7`
* borders: `BORDER_CONSTANT`, `BORDER_REPLICATE`, `BORDER_REFLECT`,
  `BORDER_REFLECT_101`, `BORDER_WRAP`

Each channel is compared with the single-channel result for the same channel
data. This also detects channel misalignment when the channel count changes.

Results:

* `8U`: maximum cross-layout difference `0` gray levels.
* `16U`: maximum cross-layout difference `0` gray levels.
* `32F`: maximum absolute difference `0.0313` on 16-bit-scaled values
  (relative error below `1e-6`, i.e. float ULP accumulation only).

Against an independent double-precision bilinear calculation:

* `8U` and `16U`: maximum difference `1` gray level, which is the unavoidable
  final rounding difference; the reference uses round-half-away-from-zero while
  the optimized path uses round-to-nearest-even.
* `32F`: maximum absolute difference `0.0311` on values up to about `32767`;
  this is floating-point accumulation order, not a coordinate or border rule
  difference.

## Stability

Dataset: `8U`, `16U`, `32F`; channels `1, 2, 3, 4, 5, 7`; five border modes;
planar floating-point maps; image size `317 x 241`.

The complete output buffer was hashed for ten independent process runs. All ten
runs produced the same SHA-256:

```
253b25a00087e9929f42d0583d15c09f2f12abddb8b4f646e135b667049a537f
```

The in-tree regression test additionally repeats the remap ten times per
layout/border combination and asserts byte-for-byte equality on every
iteration.

## Performance

`1920 x 1080`, floating-point maps, median of 15 runs, four worker threads
(milliseconds):

| Layout | BORDER_CONSTANT | BORDER_REPLICATE |
| --- | ---: | ---: |
| `8UC1` | 0.53 | 0.55 |
| `8UC2` | 0.97 | 1.04 |
| `8UC3` | 1.30 | 1.35 |
| `8UC4` | 0.65 | 0.70 |

`8UC1/8UC3/8UC4` kernels were not modified. The new `8UC2` path replaces the
legacy fixed-map fallback, whose corresponding measurements were `1.94` and
`1.89` ms on the same machine, so the two-channel path is faster as well as
consistent.

## Commands

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_LIST=imgproc,core,ts -DBUILD_TESTS=ON
cmake --build build --target opencv_test_imgproc -j
./build/bin/opencv_test_imgproc \
    --gtest_filter='Imgproc_Remap.channel_layout_consistency:Imgproc_Remap_Test.accuracy'
```
