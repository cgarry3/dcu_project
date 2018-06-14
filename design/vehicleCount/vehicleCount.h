
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
	#define ROIWIDTH     50
	#define ROIHEIGHT    25

	#define ROILeftLaneY   410
	#define ROILeftLaneX   350
	#define ROIStrideLeft  80

	#define ROIRightLaneY   410
	#define ROIRightLaneX   710
	#define ROIStrideRight  80

	#define NUMLEFTLANES   3
	#define NUMRIGHTLANES  3

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

	void vehicleCount(AXI_STREAM& stream_in, AXI_STREAM& stream_out, int& debug, int& result);

	template<typename IMG_T, typename PIXEL_T>
	void detectVehicleInLane(
				IMG_T& img_in0,
				IMG_T& img_in1,
				IMG_T& img_out,
				int rows,
				int cols,
				int& debug,
				int& result);

	void replicate(
				RGB_IMAGE& img_in,
				RGB_IMAGE& img_out0,
				RGB_IMAGE& img_out1,
				int rows,
				int cols);

#endif
