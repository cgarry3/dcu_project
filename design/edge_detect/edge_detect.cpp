#include "edge_dect.h"

 
void edge_detect(stream_t& stream_in, stream_t& stream_out)
{
	// directives
	#pragma HLS INTERFACE axis register both port=stream_out
	#pragma HLS INTERFACE axis register both port=stream_in
    #pragma HLS dataflow

 
	// Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

 
    // local storage
    rgb_img_t img0(rows, cols);
    rgb_img_t img1(rows, cols);
    rgb_img_t img2(rows, cols);
    rgb_img_t img3(rows, cols);
    rgb_img_t img4(rows, cols);
    rgb_img_t img5(rows, cols);

 
    // turn gray
    hls::AXIvideo2Mat(stream_in, img0);
    hls::CvtColor<HLS_RGB2GRAY>(img0, img1);

 
    // dilate blur
    hls::Dilate(img1, img2);

    // detect edge
    hls::Sobel<1,0,3>(img2, img3);
    hls::CvtColor<HLS_GRAY2RGB>(img3, img4);

    // output image
    hls::Mat2AXIvideo(img4, stream_out);
}
