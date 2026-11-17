#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <cstdio>

using namespace cv;

// Emits a deterministic fingerprint of whole-image (non-submatrix) denoising
// output for several type/norm combinations. Run against the pre-fix and
// post-fix libraries; identical fingerprints prove the whole-image path is
// bit-for-bit unchanged.
static unsigned long long fingerprint(const Mat& m)
{
    unsigned long long h = 1469598103934665603ULL;
    const size_t n = m.total() * m.elemSize();
    for (size_t i = 0; i < n; ++i)
    {
        h ^= m.data[i];
        h *= 1099511628211ULL;
    }
    return h;
}

static void run1(const char* name, int depth, int cn, int norm)
{
    Mat img(257, 311, CV_MAKE_TYPE(depth, cn));
    RNG(4242).fill(img, RNG::UNIFORM, 0, 256);
    Mat out;
    fastNlMeansDenoising(img, out, std::vector<float>(cn, 10.0f), 7, 21, norm);
    printf("%-18s fp=%016llx\n", name, fingerprint(out));
}

int main()
{
    run1("8UC1_L2", CV_8U, 1, NORM_L2);
    run1("8UC3_L2", CV_8U, 3, NORM_L2);
    run1("8UC1_L1", CV_8U, 1, NORM_L1);
    run1("8UC4_L1", CV_8U, 4, NORM_L1);
    run1("16UC1_L1", CV_16U, 1, NORM_L1);
    run1("16UC3_L1", CV_16U, 3, NORM_L1);
    return 0;
}
