
// -------------------------------------------------------------------------
//       DCU White Line Detect IP
//
//  Author:       Cathal Garry
//  Description:  Header file for whiteLineDetect IP
// ------------------------------------------------------------------------

#ifndef VEHICLECOUNT_H
#define VEHICLECOUNT_H

#include "hls_video.h"

	// ----------------------------------------
	//  Defines
	// ----------------------------------------

	// image defines
	#define MAX_HEIGHT 720
	#define MAX_WIDTH 1280

  // Image width and height
  int const rows = MAX_HEIGHT;
  int const cols = MAX_WIDTH;


	// ----------------------------------------
	//  Type Defines
	// ----------------------------------------

	typedef hls::stream<ap_axiu<24,1,1,1> >                 AXI_STREAM;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC3>   RGB_IMAGE;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC1>   SINGLE_IMAGE;
	typedef hls::Scalar<1, unsigned char>                   GRAY_PIXEL;
	typedef hls::Scalar<3, unsigned char>                   RGB_PIXEL;

	// ----------------------------------------
	//  Functions
	// ----------------------------------------

	void whiteLineDetect(AXI_STREAM& stream_in, AXI_STREAM& stream_out);

	template<typename IMG_T, typename PIXEL_T>
	void checkIfWhite(IMG_T& img_in,IMG_T& img_out, int rows, int cols);

	void rgbThreshold(RGB_IMAGE& img_in, RGB_IMAGE& img_out0, int cmpVal, int thresholdVal,int rows, int cols);

	void removeIllumation(RGB_IMAGE& imgIn, RGB_IMAGE& imgOut, int rows, int cols);

	void removeShadows(RGB_IMAGE& imgIn, RGB_IMAGE& imgOut, int rows, int cols);

	void removeShadowSingleFrame(SINGLE_IMAGE& imgIn, SINGLE_IMAGE& imgOut, int rows, int cols);

#endif
