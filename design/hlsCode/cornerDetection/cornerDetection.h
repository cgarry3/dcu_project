// -------------------------------------------------------------------------
//       DCU Custom Corner Detector Filter
//
//  Author:       Cathal Garry
//  Description:
// ------------------------------------------------------------------------

#ifndef EDGEDETECT_H
#define EDGEDETECT_H

#include "hls_video.h"

        // defines
	#define MAX_HEIGHT 720
	#define MAX_WIDTH  1280


	// types
	typedef hls::stream<ap_axiu<24,1,1,1> >                 AXI_STREAM;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC3>   RGB_IMAGE;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC1>   GRAY_IMAGE;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_32FC1>  HARRIS_IMAGE;
	typedef hls::Scalar<1, unsigned char>                   GRAY_PIXEL;
	typedef hls::Scalar<3, unsigned char>                   RGB_PIXEL;

	// functions
	void cornerDetection(AXI_STREAM& stream_in, AXI_STREAM& stream_out);

	template<typename IMG_T, typename PIXEL_T>
	void setCornersRed(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_out, int rows, int cols);


#endif
