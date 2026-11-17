# PNG codec round-trip gate

## Diagnosis

The failing assertion compared the exact byte length of the encoded PNG.  A
lossless PNG has no stable encoded length requirement: zlib/libpng may choose a
different, equally valid set of LZ77/Huffman matches or row filters on another
architecture or toolchain.  The decoder output is the compatibility contract.

The round-trip check now separates the two measurements:

- `byte_length_conclusion`: diagnostic only; it may be `match` or `differs`.
- `pixel_conclusion`: the blocking correctness gate; it must be `match`.

The same values are printed on stdout and written to the JUnit XML properties
via `RecordProperty`, so CI reports can distinguish an encoder-size difference
from an actual decoded-pixel regression.

## Verification data

Image: `opencv_extra/testdata/highgui/readwrite/read.png`

| Build | PNG encoder / zlib | Encoded bytes | Max abs pixel diff | Byte length | Pixels |
| --- | --- | ---: | ---: | --- | --- |
| x86_64 | libpng 1.6.57 / zlib 1.3.1 | 816219 | 0 | match | match |
| aarch64 (NEON, QEMU) | libpng 1.6.57 / zlib 1.3.2 | 816219 | 0 | match | match |
| x86_64 | libspng 0.7.4 / zlib 1.3.1 | 817407 | 0 | +1188 vs libpng | match |

The libspng row is the important control: its compressed length differs from
the libpng baseline by 1188 bytes while every decoded pixel is identical.  This
is a valid encoder implementation difference, not a codec regression.

Stability runs (the full PNG encode subset, 24 tests each):

| Architecture | Run 1 | Run 2 | Run 3 |
| --- | --- | --- | --- |
| x86_64 | pass | pass | pass |
| aarch64 (QEMU) | pass | pass | pass |

The aarch64 result is an instruction-level emulation.  Repeat the same command
on the real ARM64 build runner during the observation window.

## Gate rollout

1. Merge the test fix with the test active.  Do not use `DISABLED_` and do not
   add a skip entry.
2. Re-enable the test job immediately and keep it non-blocking for seven days.
   Record `encoded_bytes` and `max_abs_pixel_diff` from each run on both
   architectures.
3. Before making the check required, run it three consecutive times on both
   architectures and confirm every run reports `pixel_conclusion=match`.
4. Remove any temporary quarantine/skip entry, add the test to the required
   checks in branch protection, and keep the byte length as an informational
   artifact only.

## Build team commands

Build the test binary with the normal architecture-specific toolchain, then:

```bash
export OPENCV_TEST_DATA_PATH=/path/to/opencv_extra/testdata
./build/bin/opencv_test_imgcodecs \
  --gtest_filter='Imgcodecs_Png.write_big:Imgcodecs_Png_Encode.*:Imgcodecs_Png_ImwriteFlags.*' \
  --gtest_output=xml:png-roundtrip-$ARCH.xml
```

Fail the gate only when `pixel_conclusion` is not `match`.  A
`byte_length_conclusion=differs` with `pixel_conclusion=match` is expected when
the zlib/libpng implementation changes.
