#ifndef RGB2GRAYSCALE_H
#define RGB2GRAYSCALE_H

	#include  "hls_video.h"
	#include <ap_fixed.h>

    #define MAX_WIDTH  1280
    #define MAX_HEIGHT 720

    typedef hls::stream<ap_axiu<32,1,1,1> >               AXI_STREAM;
    typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC3> RGB_IMAGE;
    typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC1> GRAY_IMAGE;

	void rgb2grayScale_filter(
		AXI_STREAM& INPUT_STREAM,
		AXI_STREAM& OUTPUT_STREAM );

#endif
