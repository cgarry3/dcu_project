#include "edge_dect.h"
#include "hls_opencv.h"
#include "iostream"

using namespace std;

int main()
{
    // Width and height of image
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // load image
    cv::Mat src = cv::imread(INPUT_IMAGE);
    cv::Mat dst = src;

    // DUT
    stream_t stream_in, stream_out;
    cvMat2AXIvideo(src, stream_in);
    edge_detect(stream_in, stream_out);
    AXIvideo2cvMat(stream_out, dst);

    // write out image
    cv::imwrite(OUTPUT_IMAGE, dst);
    return 0;
}
