#include <opencv2/core.hpp>
#include <cstdio>

using namespace cv;

int main()
{
    Mat parent(40, 40, CV_8UC1);
    for (int i = 0; i < parent.rows; ++i)
        for (int j = 0; j < parent.cols; ++j)
            parent.at<uchar>(i, j) = (uchar)(i * 40 + j);

    Rect roiRect(5, 5, 20, 20);
    Mat roi(parent, roiRect);

    Mat bDefault, bIsolated;
    const int border = 8;
    copyMakeBorder(roi, bDefault, border, border, border, border, BORDER_DEFAULT);
    copyMakeBorder(roi, bIsolated, border, border, border, border,
                   BORDER_DEFAULT | BORDER_ISOLATED);

    Mat diff;
    absdiff(bDefault, bIsolated, diff);
    int nz = countNonZero(diff);
    printf("non-isolated vs isolated differing pixels: %d / %d\n",
           nz, (int)diff.total());

    // Show the bottom border rows of each to see what was sampled.
    printf("default  last row first 12:");
    for (int x = 0; x < 12; ++x) printf(" %d", bDefault.at<uchar>(bDefault.rows - 1, x));
    printf("\nisolated last row first 12:");
    for (int x = 0; x < 12; ++x) printf(" %d", bIsolated.at<uchar>(bIsolated.rows - 1, x));
    printf("\n");
    return 0;
}
