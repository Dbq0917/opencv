#include <opencv2/core.hpp>
#include <cstdio>

using namespace cv;

int main()
{
    // Parent is exactly 24 rows. Its ROI sits flush with the parent bottom.
    // copyMakeBorder without BORDER_ISOLATED still adjusts the ROI and asks
    // the border kernel for rows that do not exist in the parent allocation.
    Mat parent(24, 64, CV_8UC1, Scalar(5));
    Rect roiRect(0, 0, 64, 10);
    Mat roi(parent, roiRect);

    Mat bordered;
    const int border = 16;  // > distance from ROI bottom to parent bottom (14 rows)
    copyMakeBorder(roi, bordered, border, border, border, border, BORDER_DEFAULT);
    printf("bordered size: %d x %d\n", bordered.rows, bordered.cols);
    return 0;
}
