// -------------------------------------------------------------------------
//       DCU Custom Edge Detector Filter
//
//  Author:       Cathal Garry
//  Description:
// ------------------------------------------------------------------------

#include "cornerDetection.h"

void cornerDetection(AXI_STREAM& stream_in, AXI_STREAM& stream_out)
{

    // ----------------------------------------
	//  Local Storage
    // ----------------------------------------

	// Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // RGB Image storage
    RGB_IMAGE  imgInput(rows, cols);
    RGB_IMAGE  imgDuplicate0(rows, cols);
    RGB_IMAGE  imgDuplicate1(rows, cols);
    RGB_IMAGE  imgCorner(rows, cols);
    RGB_IMAGE  imgOutput(rows, cols);
    RGB_IMAGE  imgEdge(rows, cols);


    // Gray Image storage
    GRAY_IMAGE imgGray(rows, cols);
    GRAY_IMAGE imgNormalize(rows, cols);
    GRAY_IMAGE imgScale(rows, cols);
    GRAY_IMAGE imgThres(rows, cols);
    GRAY_IMAGE imgDilate0(rows, cols);
    GRAY_IMAGE imgDilate1(rows, cols);
    GRAY_IMAGE imgDilate2(rows, cols);
    GRAY_IMAGE imgDilate3(rows, cols);

    // Harris Filter Storage
    HARRIS_IMAGE	harris_resp(rows,cols);


    // ----------------------------------------
	//  Directives
    // ----------------------------------------

	#pragma HLS INTERFACE axis register both port=stream_out
	#pragma HLS INTERFACE axis register both port=stream_in

	// Removes ap_ctrl interface
	#pragma HLS INTERFACE ap_ctrl_none port=return

	// Synthesis data flow improvement
    #pragma HLS dataflow


    // ----------------------------------------
    //  Convert input stream
    // ----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgInput);

    // ----------------------------------------
    //  Duplicate input stream
    // ----------------------------------------

    hls::Duplicate(imgInput, imgDuplicate0, imgDuplicate1);

    // ----------------------------------------
    //  RGB to Gray
    // ----------------------------------------

    hls::CvtColor<HLS_RGB2GRAY>(imgDuplicate0, imgGray);

    // ----------------------------------------
    //  Harris Corner Filter
    // ----------------------------------------

    // Variables
	float k = 0.04;

	// Filter
	hls::CornerHarris<5,3>(imgGray, harris_resp, k);

    // ----------------------------------------
    //  Normalize Filter Output
    // ----------------------------------------

	// Variables
	float min_temp = -0.00187295f;
	float max_temp =  0.0149355f;
	int r = (int)((max_temp - min_temp) * 1000);
	float alpha = (255000.0f)/r;
	float beta = (-1)*min_temp*alpha;

	// Filter
	hls::Scale(harris_resp,imgScale,alpha,beta);

    // ----------------------------------------
    //  Threshold
    // ----------------------------------------

	hls::Threshold(imgScale, imgThres, 40, 255, HLS_THRESH_BINARY);

    // ----------------------------------------
    //  Dilate
    // ----------------------------------------

	hls::Dilate(imgThres, imgDilate0);
	hls::Dilate(imgDilate0, imgDilate1);
	hls::Dilate(imgDilate1, imgDilate2);
	hls::Dilate(imgDilate2, imgDilate3);

    // ----------------------------------------
    //  Gray to RGB
    // ----------------------------------------

    hls::CvtColor<HLS_GRAY2RGB>(imgDilate3, imgCorner);


    // ----------------------------------------
    //  Edge Detection
    // ----------------------------------------

    edge_detect<RGB_IMAGE>(imgDuplicate1, imgEdge);

    // ----------------------------------------
    //  Combine Corner and Edges
    // ----------------------------------------

    combineCornersEdges<RGB_IMAGE,RGB_PIXEL>(imgCorner, imgEdge, imgOutput, rows, cols);

    // ----------------------------------------
    //  Output image
    // ----------------------------------------

    hls::Mat2AXIvideo(imgOutput, stream_out);
}

// -----------------------------------
//  Set Corners Red
// -----------------------------------

template<typename IMG_T, typename PIXEL_T>
void combineCornersEdges(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_out, int rows, int cols) {

	PIXEL_T pin0, pin1;
	PIXEL_T pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=1 max=1080
		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=1920
		#pragma HLS pipeline rewind

				   img_in0 >> pin0;
				   img_in1 >> pin1;

				   // set pixel red
				   if(pin0.val[0]>200){
						pout.val[0] = 0;
						pout.val[1] = 0;
						pout.val[2] = 255;
				   }
				   // pass through pixel
				   else{
						pout.val[0] = pin1.val[0];
						pout.val[1] = pin1.val[1];
						pout.val[2] = pin1.val[2];
				   }

				   img_out << pout;
				}
		}

}

// -----------------------------------
//  Edge Detection
// -----------------------------------

template<typename IMG_T>
void edge_detect(IMG_T& imgInput, IMG_T& imgOutput)
{
    // Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // local storage
    // Single Frame
    GRAY_IMAGE imgGray(rows, cols);
    GRAY_IMAGE imgDilate(rows, cols);
    GRAY_IMAGE imgEdges0(rows, cols);
    GRAY_IMAGE imgEdges1(rows, cols);
    GRAY_IMAGE imgDuplicate0(rows, cols);
    GRAY_IMAGE imgDuplicate1(rows, cols);
    GRAY_IMAGE imgScale0(rows, cols);
    GRAY_IMAGE imgScale1(rows, cols);
    GRAY_IMAGE imgGrad(rows, cols);


	// Adding delay logic
	#pragma HLS stream depth=20000 variable=imgDuplicate0.data_stream
	#pragma HLS stream depth=20000 variable=imgDuplicate1.data_stream

    // Impoves synthesis flow
    #pragma HLS dataflow

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

    hls::CvtColor<HLS_GRAY2RGB>(imgGrad, imgOutput);

}
