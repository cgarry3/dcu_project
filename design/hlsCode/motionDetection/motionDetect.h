
// -------------------------------------------------------------------------
//       DCU Vehicle counter IP
//
//  Author:       Cathal Garry
//  Description:  Header file for counting the number of
//                vehicles on a motorway
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

	// function defines
    #define MOTIONBOXSIZE 40

    // Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

	// constant values
	const int motionBoxSize     = MOTIONBOXSIZE;
	const int numOfMotionBoxes  = ((rows*cols)/2)/(motionBoxSize*motionBoxSize);
	const int numOfTrackingInts = (numOfMotionBoxes/32) + 1;


	// ----------------------------------------
	//  Type Defines
	// ----------------------------------------

	typedef hls::stream<ap_axiu<24,1,1,1> >                 AXI_STREAM;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC3>   RGB_IMAGE;
	typedef hls::Mat<MAX_HEIGHT,   MAX_WIDTH,   HLS_8UC1>   SINGLE_IMAGE;
	typedef hls::Scalar<1, unsigned char>                   GRAY_PIXEL;
	typedef hls::Scalar<3, unsigned char>                   RGB_PIXEL;

	typedef ap_fixed<8,4,AP_RND>                            COEF_T;
	typedef ap_int<16>                                      INDEX_T;

	// kernel size
	const int KS = 7;

	// ----------------------------------------
	//  Functions
	// ----------------------------------------

	void motionDetect(AXI_STREAM& stream_in, AXI_STREAM& stream_out);

	void opticalFlow(RGB_IMAGE& imgIn0, RGB_IMAGE& imgIn1, RGB_IMAGE& imgOut, int rows, int cols);

	template<typename IMG_T>
	void thinLine_filter_45Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T>
	void thickLine_filter_45Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T>
	void thinLine_filter_90Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T>
	void thickLine_filter_90Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T>
	void thinLine_filter_135Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T>
	void thickLine_filter_135Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T, typename PIXEL_T>
	void andImages(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T, typename PIXEL_T>
	void orImages(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T, typename PIXEL_T>
	void clearImageTop(IMG_T& img_in0, IMG_T& img_out, int rows, int cols);

	template<typename IMG_T, typename PIXEL_T>
	void replicate_by3(IMG_T& img_in, IMG_T& img_out0, IMG_T& img_out1, IMG_T& img_out2, int rows, int cols);

	void rgbThreshold(RGB_IMAGE& img_in, RGB_IMAGE& img_out0, int cmpVal, int thresholdVal,int rows, int cols);

#endif
