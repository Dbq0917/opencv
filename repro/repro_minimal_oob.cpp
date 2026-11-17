#include <opencv2/core.hpp>
#include <cstdio>
#include <cstdlib>

using namespace cv;

int main()
{
    // Allocation holds a 32x32 image (step 32). Parent header claims 40
    // columns, so any access to columns [32, 40) walks out of the heap block.
    const int allocCols = 32, allocRows = 32;
    uchar* raw = (uchar*)std::malloc((size_t)allocRows * allocCols);
    Mat parent(allocRows, allocCols + 8, CV_8UC1, raw);
    Mat roi(parent, Rect(0, 0, allocCols, allocRows));

    Mat bordered;
    copyMakeBorder(roi, bordered, 4, 4, 4, 4, BORDER_DEFAULT);  // non-isolated
    printf("done %d x %d\n", bordered.rows, bordered.cols);
    std::free(raw);
    return 0;
}
