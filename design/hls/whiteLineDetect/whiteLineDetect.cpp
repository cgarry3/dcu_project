// -------------------------------------------------------------------------
//       DCU Vehicle counter IP
//
//  Author:       Cathal Garry
//  Description:  Custom IP for detecting white lines in an image
// ------------------------------------------------------------------------

#include "whiteLineDetect.h"

void whiteLineDetect(AXI_STREAM& stream_in, AXI_STREAM& stream_out)
{
	// ----------------------------------------
	//   directives
	// ----------------------------------------

	// creates AXI stream ports
	#pragma HLS INTERFACE axis register both port=stream_out
	#pragma HLS INTERFACE axis register both port=stream_in

	// creates AXIS registers
	#pragma HLS INTERFACE s_axilite port=result
	#pragma HLS INTERFACE s_axilite port=debug

    // Removes ap_ctrl interface
	#pragma HLS INTERFACE ap_ctrl_none port=return

	// Impoves synthesis flow
    #pragma HLS dataflow

	// ----------------------------------------
	//   Constants
	// ----------------------------------------

	// Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;


	// ----------------------------------------
	//   Local Storage
	// ----------------------------------------


    RGB_IMAGE 	 imgRGB      		(rows, cols);
    RGB_IMAGE 	 imgShadow      	(rows, cols);
    RGB_IMAGE 	 imgIllum      		(rows, cols);
    RGB_IMAGE 	 imgHSV0  		    (rows, cols);
    RGB_IMAGE 	 imgHSV1  		    (rows, cols);
    RGB_IMAGE 	 imgOut  		    (rows, cols);


    // ----------------------------------------
    //  Stage 1: Input Stream
    // ----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgRGB);


    // ----------------------------------------
    //  Stage 2: Remove Shadow + illumation (optional)
    // ----------------------------------------

    //removeShadows(imgRGB, imgShadow, rows, cols);
    //removeIllumation(imgShadow, imgIllum, rows, cols);

    // ----------------------------------------
    //  Stage 3: Convert to HSV
    // ----------------------------------------

    hls::CvtColor<HLS_RGB2HSV>(imgRGB,imgHSV0);

    // ----------------------------------------
    //  Stage 4: Find white lines
    // ----------------------------------------

    checkIfWhite<RGB_IMAGE,RGB_PIXEL>(imgHSV0, imgHSV1, rows, cols);

    // ----------------------------------------
    //  Stage 5: Convert to RGB
    // ----------------------------------------

    hls::CvtColor<HLS_HSV2RGB>(imgHSV1,imgOut);

    // ----------------------------------------
    // Stage 6: output image
    // ----------------------------------------

    hls::Mat2AXIvideo(imgOut, stream_out);
}


// -----------------------------------
// Threshold function for RGB image
// -----------------------------------

void rgbThreshold(RGB_IMAGE& img_in, RGB_IMAGE& img_out0, int cmpVal, int thresholdVal,int rows, int cols)
{
	SINGLE_IMAGE imgSplitCh0 		(rows, cols);
	SINGLE_IMAGE imgSplitCh1 		(rows, cols);
	SINGLE_IMAGE imgSplitCh2 		(rows, cols);
	SINGLE_IMAGE imgThresholdCh0    (rows, cols);
	SINGLE_IMAGE imgThresholdCh1    (rows, cols);
	SINGLE_IMAGE imgThresholdCh2    (rows, cols);
	RGB_IMAGE 	 imgOut    			(rows, cols);

	// split RGB
	hls::Split(img_in, imgSplitCh0, imgSplitCh1, imgSplitCh2);

	// Threshold
	hls::Threshold(imgSplitCh0, imgThresholdCh0, cmpVal, thresholdVal, HLS_THRESH_BINARY );
	hls::Threshold(imgSplitCh1, imgThresholdCh1, cmpVal, thresholdVal, HLS_THRESH_BINARY );
	hls::Threshold(imgSplitCh2, imgThresholdCh2, cmpVal, thresholdVal, HLS_THRESH_BINARY );

	// Merging RGB channels for output
	hls::Merge( imgThresholdCh0, imgThresholdCh1, imgThresholdCh2, img_out0 );

}

// -----------------------------------
//  Check for white pixels
// -----------------------------------

template<typename IMG_T, typename PIXEL_T>
void checkIfWhite(IMG_T& img_in,IMG_T& img_out, int rows, int cols) {

	PIXEL_T pin;
	PIXEL_T pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=1 max=720
		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=1280
		#pragma HLS pipeline rewind

				   // Input Pixel
				   img_in >> pin;

				   if(pin.val[1]<20 && pin.val[2]>180)
				   {
					  pout = pin;
				   }
				   else
				   {
					  pout = 0;
				   }


				   // Output Pixel
				   img_out << pout;
		}
	}

}

// -----------------------------------
//  Remove illumation in image
// -----------------------------------

void removeIllumation(RGB_IMAGE& imgIn, RGB_IMAGE& imgOut, int rows, int cols)
{
		// --------------------------------------------------
		// Image Storage
		// --------------------------------------------------

		SINGLE_IMAGE    imgSplitCh0 		(rows, cols);
		SINGLE_IMAGE    imgSplitCh1 		(rows, cols);
		SINGLE_IMAGE    imgSplitCh2 		(rows, cols);
		SINGLE_IMAGE    imgEqual     		(rows, cols);
		RGB_IMAGE       YCrCb_IMAGE_in      (rows, cols);
		RGB_IMAGE       YCrCb_IMAGE_merge  (rows, cols);

		// --------------------------------------------------
        // Split chrome and luma
		// --------------------------------------------------

		hls::CvtColor<HLS_RGB2YCrCb>(imgIn, YCrCb_IMAGE_in);

		// --------------------------------------------------
		// Split channels
		// --------------------------------------------------

		hls::Split(YCrCb_IMAGE_in, imgSplitCh0, imgSplitCh1, imgSplitCh2);

		// --------------------------------------------------
		// Equalize chroma channle
		// --------------------------------------------------

		hls::EqualizeHist(imgSplitCh0, imgEqual);

		// --------------------------------------------------
		// Merge channels
		// --------------------------------------------------

		hls::Merge( imgEqual, imgSplitCh1, imgSplitCh2,YCrCb_IMAGE_merge );

		// --------------------------------------------------
		// Convert back to RGB
		// --------------------------------------------------

		hls::CvtColor<HLS_YCrCb2RGB>(YCrCb_IMAGE_merge, imgOut);
}

// -----------------------------------
//  Remove shadow in image
// -----------------------------------

void removeShadows(RGB_IMAGE& imgIn, RGB_IMAGE& imgOut, int rows, int cols)
{

	// --------------------------------------------------
	// Image Storage
	// --------------------------------------------------

	SINGLE_IMAGE    imgSplitCh0In 		(rows, cols);
	SINGLE_IMAGE    imgSplitCh1In 		(rows, cols);
	SINGLE_IMAGE    imgSplitCh2In 		(rows, cols);
	SINGLE_IMAGE    imgSplitCh0Out 		(rows, cols);
	SINGLE_IMAGE    imgSplitCh1Out 		(rows, cols);
	SINGLE_IMAGE    imgSplitCh2Out 		(rows, cols);
	SINGLE_IMAGE    imgEqual     		(rows, cols);

	// --------------------------------------------------
	// Split channels
	// --------------------------------------------------

	hls::Split(imgIn, imgSplitCh0In, imgSplitCh1In, imgSplitCh2In);

	// --------------------------------------------------
	// Remove shadows from single frames
	// --------------------------------------------------

	removeShadowSingleFrame(imgSplitCh0In, imgSplitCh0Out, rows, cols);
	removeShadowSingleFrame(imgSplitCh1In, imgSplitCh1Out, rows, cols);
	removeShadowSingleFrame(imgSplitCh2In, imgSplitCh2Out, rows, cols);

	// --------------------------------------------------
	// Merge channels
	// --------------------------------------------------

	hls::Merge( imgSplitCh0Out, imgSplitCh1Out, imgSplitCh2Out,imgOut );
}

void removeShadowSingleFrame(SINGLE_IMAGE& imgIn, SINGLE_IMAGE& imgOut, int rows, int cols)
{
		// --------------------------------------------------
		// Image Storage
		// --------------------------------------------------

		SINGLE_IMAGE       imgDuplicate0    (rows, cols);
		SINGLE_IMAGE       imgDuplicate1    (rows, cols);
		SINGLE_IMAGE       imgDilate        (rows, cols);
		SINGLE_IMAGE       imgBlur          (rows, cols);
		SINGLE_IMAGE       imgDiff          (rows, cols);

    // --------------------------------------------------
		//  Removing Shadow
		// --------------------------------------------------

		hls::Duplicate(imgIn, imgDuplicate0, imgDuplicate1);
        hls::Dilate(imgDuplicate0,imgDilate);
        hls::GaussianBlur<21,21>(imgDilate,imgBlur);
        hls::AbsDiff(imgBlur, imgDuplicate1, imgDiff);


        GRAY_PIXEL pin;
        GRAY_PIXEL pout;

        L_row: for(int row = 0; row < rows; row++) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=720

          L_col: for(int col = 0; col < cols; col++) {
          #pragma HLS LOOP_TRIPCOUNT min=1 max=1280
          #pragma HLS pipeline rewind

                     // Input Pixel
               imgDiff    >> pin;

               // Processing output pixel
               pout.val[0] =  255 - pin.val[0];

                       // Output Pixel
                       imgOut << pout;
                    }
                }
}

