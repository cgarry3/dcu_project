// -------------------------------------------------------------------------
//       DCU Custom Edge Detector Filter
//
//  Author:       Cathal Garry
//  Description:  Header file for custom edge detector filter for
//                720P images.
// ------------------------------------------------------------------------

#ifndef EDGEDETECT_H
#define EDGEDETECT_H

#include "hls_video.h"

    // defines
	#define MAX_HEIGHT 720
	#define MAX_WIDTH 1280


	// types
	typedef hls::stream<ap_axiu<24,1,1,1> >                 AXI_STREAM;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC3>   RGB_IMAGE;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC1>   GRAY_IMAGE;
	typedef hls::Scalar<1, unsigned char>                   GRAY_PIXEL;

	// functions
	void edge_detect(
			AXI_STREAM& stream_in,
			AXI_STREAM& stream_out);


#endif
