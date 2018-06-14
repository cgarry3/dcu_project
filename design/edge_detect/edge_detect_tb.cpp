// -------------------------------------------------------------------------
//       DCU Custom Edge Detector Filter
//
//  Author:       Cathal Garry
//  Description:  Testbench for custom edge detector filter for
//                720P images.
// ------------------------------------------------------------------------

#include "edge_dect.h"
#include "hls_opencv.h"
#include "iostream"

using namespace std;

// inputs and output files
#define INPUT_IMAGE "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_Traffic.jpg"
#define OUTPUT_IMAGE "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\edge_detect.jpg"


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
    edge_detect(stream_in, stream_out);
    AXIvideo2cvMat(stream_out, dst);

    // write out image
    cv::imwrite(OUTPUT_IMAGE, dst);
    return 0;
}

