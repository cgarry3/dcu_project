// -------------------------------------------------------------------------
//       DCU Custom Corner Detector Filter
//
//  Author:       Cathal Garry
//  Description:
// ------------------------------------------------------------------------

#include "cornerDetection.h"
#include "hls_opencv.h"
#include "iostream"

using namespace std;

// inputs and output files
#define INPUT_IMAGE "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\output2.jpg"
#define OUTPUT_IMAGE "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\cornerDetection.jpg"


int main()
{
    // Width and height of image
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // load image
    cv::Mat src = cv::imread(INPUT_IMAGE);
    cv::Mat dst = src;

    // DUT
    AXI_STREAM stream_in, stream_out;
    cvMat2AXIvideo(src, stream_in);
    cornerDetection(stream_in, stream_out);
    AXIvideo2cvMat(stream_out, dst);

    // write out image
    cv::imwrite(OUTPUT_IMAGE, dst);
    return 0;
}
