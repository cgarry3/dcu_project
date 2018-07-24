// -------------------------------------------------------------------------
//       DCU Custom Edge Detector Filter
//
//  Author:       Cathal Garry
//  Description:  This is a custom edge detector filter for
//                720P images. It contains an input and output AXI stream
//                ports.
// ------------------------------------------------------------------------

#include "edge_dect.h"

void edge_detect(AXI_STREAM& stream_in, AXI_STREAM& stream_out)
{
    // directives
    #pragma HLS INTERFACE axis register both port=stream_out
    #pragma HLS INTERFACE axis register both port=stream_in

    // Removes ap_ctrl interface
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Synthesis data flow improvement
    #pragma HLS dataflow

	// Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // local storage
    RGB_IMAGE  imgInput(rows, cols);
    GRAY_IMAGE imgGray(rows, cols);
    GRAY_IMAGE imgDilate(rows, cols);
    GRAY_IMAGE imgEdges(rows, cols);
    RGB_IMAGE  imgOutput(rows, cols);

    // convert input stream
    hls::AXIvideo2Mat(stream_in, imgInput);

    // turn gray
    hls::CvtColor<HLS_RGB2GRAY>(imgInput, imgGray);

    // dilate blur
    hls::Dilate(imgGray, imgDilate);

    // detect edge
    hls::Sobel<1,0,3>(imgDilate, imgEdges);
    hls::CvtColor<HLS_GRAY2RGB>(imgEdges, imgOutput);;

    // output image
    hls::Mat2AXIvideo(imgOutput, stream_out);
}
