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

typedef ap_axiu<24,1,1,1> interface_t;
typedef hls::stream<interface_t> stream_t;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC3> rgb_img_t;
typedef hls::Mat<MAX_HEIGHT, MAX_WIDTH, HLS_8UC1> single_img_t;
typedef hls::Scalar<3, unsigned char>             rgb_pixel_t;

// ----------------------------------------
//  Functions
// ----------------------------------------

void vehicleCount(stream_t& stream_in, stream_t& stream_out, int& result);

template<typename IMG_T, typename PIXEL_T>
void detectVehicleInLane(
	IMG_T& img_in0,
	IMG_T& img_out,
	int rows,
	int cols,
	int& result);
