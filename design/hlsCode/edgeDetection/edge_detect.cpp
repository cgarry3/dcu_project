// -------------------------------------------------------------------------
//       DCU Custom Edge Detector Filter
//
//  Author:       Cathal Garry
//  Description:  This is a custom edge detector filter. It contains
//                an input and output AXI stream ports.
//
// ------------------------------------------------------------------------

#include "edge_dect.h"

void edge_detect(AXI_STREAM& stream_in, AXI_STREAM& stream_out)
{
    // Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // local storage
    RGB_IMAGE  imgInput(rows, cols);
    GRAY_IMAGE imgGray(rows, cols);
    GRAY_IMAGE imgDilate(rows, cols);
    GRAY_IMAGE imgEdges0(rows, cols);
    GRAY_IMAGE imgEdges1(rows, cols);
    GRAY_IMAGE imgDuplicate0(rows, cols);
    GRAY_IMAGE imgDuplicate1(rows, cols);
    GRAY_IMAGE imgScale0(rows, cols);
    GRAY_IMAGE imgScale1(rows, cols);
    GRAY_IMAGE imgGrad(rows, cols);
    RGB_IMAGE  imgOutput(rows, cols);

    // directives
    #pragma HLS INTERFACE axis register both port=stream_out
    #pragma HLS INTERFACE axis register both port=stream_in

    // Removes ap_ctrl interface
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Synthesis data flow improvement
    #pragma HLS dataflow

    // Adding delay logic 
    #pragma HLS stream depth=20000 variable=imgDuplicate0.data_stream
    #pragma HLS stream depth=20000 variable=imgDuplicate1.data_stream

    // -----------------------------------------
    //  Input stream
    // -----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgInput);

    // -----------------------------------------
    //  RGB to Grayscale
    // -----------------------------------------

    hls::CvtColor<HLS_RGB2GRAY>(imgInput, imgGray);

    // -----------------------------------------
    //  Dilate blur
    // -----------------------------------------

    hls::Dilate(imgGray, imgDilate);

    // -----------------------------------------
    //  Duplicate
    // -----------------------------------------

    hls::Duplicate(imgDilate, imgDuplicate0, imgDuplicate1);

    // -----------------------------------------
    //  Sobel Filters
    // -----------------------------------------

    // X - Gradient
    hls::Sobel<0,1,3>(imgDuplicate0, imgEdges0);
    // Y - Gradient
    hls::Sobel<1,0,3>(imgDuplicate1, imgEdges1);

    // -----------------------------------------
    //  Convert back to CV_8U
    // -----------------------------------------

    hls::ConvertScaleAbs(imgEdges0, imgScale0);
    hls::ConvertScaleAbs(imgEdges1, imgScale1);

    // -----------------------------------------
    //  Approximate the Gradient
    // -----------------------------------------

    hls::AddWeighted( imgScale0, double(0.5), imgScale1, double(0.5), double(0.0), imgGrad );

    // -----------------------------------------
    //  GrayScale to RGB
    // -----------------------------------------

    hls::CvtColor<HLS_GRAY2RGB>(imgGrad, imgOutput);;

    // -----------------------------------------
    //  Output Stream
    // -----------------------------------------

    hls::Mat2AXIvideo(imgOutput, stream_out);
}
