// -------------------------------------------------------------------------
//       DCU Custom RGB to Grayscale Filter
//
//  Author:       Cathal Garry
//  Description:  This is a custom RGB to Grayscale filter for
//                720P images. It contains an input and output AXI stream
//                ports.
// -------------------------------------------------------------------------

#ifndef RGB2GRAYSCALE_H
#define RGB2GRAYSCALE_H

    #include  "hls_video.h"
    #include <ap_fixed.h>

    // defines
    #define MAX_WIDTH  1280
    #define MAX_HEIGHT 720

    // types
    typedef hls::stream<ap_axiu<24,1,1,1> >               AXI_STREAM;
    typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC3> RGB_IMAGE;
    typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC1> GRAY_IMAGE;
    typedef hls::Scalar<1, unsigned char>                 GRAY_PIXEL;

    // functions
    void rgb2grayScale_filter(
		AXI_STREAM& INPUT_STREAM,
		AXI_STREAM& OUTPUT_STREAM );


#endif
