// -------------------------------------------------------------------------
//       DCU Motion Detector IP
//
//  Author:       Cathal Garry
//  Description:  Custom IP for detection motion
// ------------------------------------------------------------------------

#include "motionDetect.h"

void motionDetect(AXI_STREAM& stream_in, AXI_STREAM& stream_out)
{
    // ----------------------------------------
    //   Directives
    // ----------------------------------------

    // creates AXI stream ports
    #pragma HLS INTERFACE axis register both port=stream_out
    #pragma HLS INTERFACE axis register both port=stream_in
	
    // Removes ap_ctrl interface
    #pragma HLS INTERFACE ap_ctrl_none port=return

    // Impoves synthesis flow
    #pragma HLS dataflow



    // ----------------------------------------
    //   Local Storage
    // ----------------------------------------

    // RGB images
    RGB_IMAGE 	 imgRGB      		(rows, cols);
    RGB_IMAGE 	 imgDuplicateRGB0       (rows, cols);
    RGB_IMAGE 	 imgDuplicateRGB1       (rows, cols);
    RGB_IMAGE 	 imgLines               (rows, cols);
    RGB_IMAGE 	 imgOut      		(rows, cols);

    // Single frame images
    SINGLE_IMAGE 	 imgGray     			(rows, cols);
    SINGLE_IMAGE 	 imgDilate  			(rows, cols);
    SINGLE_IMAGE 	 imgEdges   			(rows, cols);
    SINGLE_IMAGE 	 imgThres    			(rows, cols);
    SINGLE_IMAGE 	 imgThresLine45Thin    	        (rows, cols);
    SINGLE_IMAGE 	 imgThresLine45Thick            (rows, cols);
    SINGLE_IMAGE 	 imgThresLine90Thin    	        (rows, cols);
    SINGLE_IMAGE 	 imgThresLine90Thick            (rows, cols);
    SINGLE_IMAGE 	 imgThresLine135Thin            (rows, cols);
    SINGLE_IMAGE 	 imgThresLine135Thick           (rows, cols);
    SINGLE_IMAGE 	 imgClear    			(rows, cols);
    SINGLE_IMAGE 	 imgDuplicate0    		(rows, cols);
    SINGLE_IMAGE 	 imgDuplicate1    		(rows, cols);
    SINGLE_IMAGE 	 imgDuplicate2    		(rows, cols);
    SINGLE_IMAGE 	 imgLineThin45    		(rows, cols);
    SINGLE_IMAGE 	 imgLineThick45    		(rows, cols);
    SINGLE_IMAGE 	 imgLineThin90    		(rows, cols);
    SINGLE_IMAGE 	 imgLineThick90    		(rows, cols);
    SINGLE_IMAGE 	 imgLineThin135    		(rows, cols);
    SINGLE_IMAGE 	 imgLineThick135    	        (rows, cols);
    SINGLE_IMAGE 	 imgOR0 		   	(rows, cols);
    SINGLE_IMAGE 	 imgOR1	    			(rows, cols);



    // ----------------------------------------
    //  Stage 1:  Remove colour
    // ----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgRGB);
    hls::Duplicate(imgRGB, imgDuplicateRGB0, imgDuplicateRGB1);
    hls::CvtColor<HLS_RGB2GRAY>(imgDuplicateRGB0, imgGray);

    // ----------------------------------------
    //  Stage 2: Dilate
    // ----------------------------------------

    hls::Dilate(imgGray, imgDilate);

    // ----------------------------------------
    //  Stage 3:  Edge Detection
    // ----------------------------------------

    hls::Sobel<1,0,3>(imgDilate, imgEdges);


    // ----------------------------------------
    //  Stage 4: Threshold
    // ----------------------------------------

    hls::Threshold(imgEdges, imgThres, 150, 2, HLS_THRESH_BINARY);


    // ----------------------------------------
    //  Stage 5: Clear top half of image
    // ----------------------------------------

    clearImageTop<SINGLE_IMAGE, GRAY_PIXEL>(imgThres, imgClear, rows, cols);

    // ----------------------------------------
    //  Stage 6: Line Filters
    // ----------------------------------------

    // Split frame for parellal processing
    replicate_by3<SINGLE_IMAGE, GRAY_PIXEL>(imgClear, imgDuplicate0, imgDuplicate1, imgDuplicate2, rows, cols);


    // 45 degrees line filter
    thinLine_filter_45Degrees<SINGLE_IMAGE>(imgDuplicate0, imgLineThin45, rows, cols);
    hls::Threshold(imgLineThin45, imgThresLine45Thin, 20, 2, HLS_THRESH_BINARY);
    thickLine_filter_45Degrees<SINGLE_IMAGE>(imgThresLine45Thin, imgLineThick45, rows, cols);
    hls::Threshold(imgLineThick45, imgThresLine45Thick, 20, 255, HLS_THRESH_BINARY);

    // 90 degrees line filter
    thinLine_filter_90Degrees<SINGLE_IMAGE>(imgDuplicate1, imgLineThin90, rows, cols);
    hls::Threshold(imgLineThin90, imgThresLine90Thin, 30, 2, HLS_THRESH_BINARY);
    thickLine_filter_90Degrees<SINGLE_IMAGE>(imgThresLine90Thin, imgLineThick90, rows, cols);
    hls::Threshold(imgLineThick90, imgThresLine90Thick, 30, 255, HLS_THRESH_BINARY);

    // 135 degrees line filter
    thinLine_filter_135Degrees<SINGLE_IMAGE>(imgDuplicate2, imgLineThin135, rows, cols);
    hls::Threshold(imgLineThin135, imgThresLine135Thin, 20, 2, HLS_THRESH_BINARY);
    thickLine_filter_135Degrees<SINGLE_IMAGE>(imgThresLine135Thin, imgLineThick135, rows, cols);
    hls::Threshold(imgLineThick135, imgThresLine135Thick, 20, 255, HLS_THRESH_BINARY);

    // OR together all three images
    orImages<SINGLE_IMAGE, GRAY_PIXEL>(imgThresLine135Thick, imgThresLine90Thick, imgOR0, rows, cols);
    orImages<SINGLE_IMAGE, GRAY_PIXEL>(imgOR0 , imgThresLine45Thick, imgOR1, rows, cols);


    // convert back to RGB
    hls::CvtColor<HLS_GRAY2RGB>(imgOR1, imgLines);

    // ----------------------------------------
    //  Stage 7: Motion Detection
    // ----------------------------------------

    opticalFlow(imgLines, imgDuplicateRGB1, imgOut, rows, cols);

    // ----------------------------------------
    // output image
    // ----------------------------------------

    hls::Mat2AXIvideo(imgOut, stream_out);
}



// -----------------------------------
//  Motion Detection
// -----------------------------------

void opticalFlow(RGB_IMAGE& imgIn0, RGB_IMAGE& imgIn1, RGB_IMAGE& imgOut, int rows, int cols)
{

   // local pixel storage
   RGB_PIXEL pin1;
   RGB_PIXEL  pin0;
   RGB_PIXEL  pout;


    // variables for tracking motion
    static unsigned short int motionDetectPrevResult [numOfMotionBoxes];
    static unsigned short int motionDetectPresResult [numOfMotionBoxes];
    static unsigned int trackingInts                 [numOfTrackingInts];
    static unsigned int frameCnt=1;

    // copy present value to previous array of values
    for(int i=0; i<numOfMotionBoxes; i++){
		   motionDetectPrevResult[i] = motionDetectPresResult[i];
		   // clear old results
		   motionDetectPresResult[i] = 0;
    }


	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=1 max=1080

		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=1920
		#pragma HLS pipeline rewind

			       // ------------------------------------
			       //  Stage 0: Input pixel
			       // ------------------------------------

				   imgIn0 >> pin0;
				   imgIn1 >> pin1;

			       // ------------------------------------
			       //  Stage 1: Current Motion Box and tracker number
			       // ------------------------------------

				   int motionBoxNum = (col/motionBoxSize) + ((row/motionBoxSize)*(cols/motionBoxSize));
                                   int trackerNum   = (motionBoxNum/32);

				   // ------------------------------------
				   //   Stage 2: Set motion box to red
				   // ------------------------------------

				   int isSet = (trackingInts[trackerNum] >> (motionBoxNum%32)) && 0x1;


				   // see if motion box is set
				   if(isSet==0x1)
				   {
					   pout.val[0] = 0;
					   pout.val[1] = 0;
					   pout.val[2] = pin1.val[0];
				   }
				   else{
					   pout.val[0] = pin1.val[0];
					   pout.val[1] = pin1.val[1];
					   pout.val[2] = pin1.val[2];
				   }

				   // ------------------------------------
				   //   Stage 3: Motion Detection
				   // ------------------------------------

				   // Last pixel in a motion box
                                   if(((col%motionBoxSize)==motionBoxSize-1) && ((row%motionBoxSize)==motionBoxSize-1)){
					   // add pixel value to count
					   if(pin0.val[0]>200){
						   motionDetectPresResult[motionBoxNum] = motionDetectPresResult[motionBoxNum] + 1;
					   }

					   int SAD = abs(motionDetectPrevResult[motionBoxNum] - motionDetectPresResult[motionBoxNum]);

					   if(SAD>10)
					   {
						   trackingInts[trackerNum] = trackingInts[trackerNum] + (1 << (motionBoxNum%32));
					   }
				   }
				   // Any other pixel
				   else if(pin0.val[0]>200)
				   {
					   motionDetectPresResult[motionBoxNum] = motionDetectPresResult[motionBoxNum] + 1;
				   }

				   // ------------------------------------
				   //  Stage 4: Output pixel
				   // ------------------------------------

				   imgOut << pout;

				}
		}

	// increment frame count
	frameCnt++;

}


// -----------------------------------
// 	45 Degree Thin Line Filter
// -----------------------------------

template<typename IMG_T>
void thinLine_filter_45Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols ) {

	// 2D kernel for 45 degree lines
	const COEF_T coef_h[KS][KS]= {
	  {-1, -1, -1, -1, -1,  2,  2},
	  {-1, -1, -1, -1,  2,  2,  2},
	  {-1, -1, -1,  2,  2,  2, -1},
	  {-1, -1,  2,  2,  2, -1, -1},
	  {-1,  2,  2,  2, -1, -1, -1},
	  { 2,  2,  2, -1, -1, -1, -1},
	  { 2,  2, -1, -1, -1, -1, -1}
	};

	hls::Window<KS,KS,COEF_T> Sh;
	for (int r=0; r<KS; r++) for (int c=0; c<KS; c++) Sh.val[r][c] = coef_h[r][c];

	// point
	hls::Point_<INDEX_T> anchor;
	anchor.x=-1;
	anchor.y=-1;

	hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sh, anchor);

}

// -----------------------------------
// 	45 Degree Thick Line Filter
// -----------------------------------

template<typename IMG_T>
void thickLine_filter_45Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols ) {

	// 2D kernel for 45 degree lines
	const COEF_T coef_h[KS][KS]= {
	  {-1, -1, -1, -1,  2,  2,  2},
	  {-1, -1, -1,  2,  2,  2,  2},
	  {-1, -1,  2,  2,  2,  2,  2},
	  {-1,  2,  2,  2,  2,  2, -1},
	  {2,   2,  2,  2,  2, -1, -1},
	  {2,   2,  2,  2, -1, -1, -1},
	  {2,   2,  2, -1, -1, -1, -1}
	};

	hls::Window<KS,KS,COEF_T> Sh;
	for (int r=0; r<KS; r++) for (int c=0; c<KS; c++) Sh.val[r][c] = coef_h[r][c];

	// point
	hls::Point_<INDEX_T> anchor;
	anchor.x=-1;
	anchor.y=-1;

	hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sh, anchor);

}

// -----------------------------------
//   90 Degree Thin Line Filter
// -----------------------------------

template<typename IMG_T>
void thinLine_filter_90Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols ) {


	// 2D kernel for 90 degree lines
	const COEF_T coef_h[KS][KS]= {
	  {-1, -1, 2, 2, 2, -1, -1},
	  {-1, -1, 2, 2, 2, -1, -1},
	  {-1, -1, 2, 2, 2, -1, -1},
	  {-1, -1, 2, 2, 2, -1, -1},
	  {-1, -1, 2, 2, 2, -1, -1},
	  {-1, -1, 2, 2, 2, -1, -1},
	  {-1, -1, 2, 2, 2, -1, -1},
	};

	hls::Window<KS,KS,COEF_T> Sh;
	for (int r=0; r<KS; r++) for (int c=0; c<KS; c++) Sh.val[r][c] = coef_h[r][c];

	// point
	hls::Point_<INDEX_T> anchor;
	anchor.x=-1;
	anchor.y=-1;

	hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sh, anchor);

}

// -----------------------------------
//   90 Degree Thick Line Filter
// -----------------------------------

template<typename IMG_T>
void thickLine_filter_90Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols ) {


	// 2D kernel for 90 degree lines
	const COEF_T coef_h[KS][KS]= {
	  {-1, 2, 2, 2, 2, 2, -1},
	  {-1, 2, 2, 2, 2, 2, -1},
	  {-1, 2, 2, 2, 2, 2, -1},
	  {-1, 2, 2, 2, 2, 2, -1},
	  {-1, 2, 2, 2, 2, 2, -1},
	  {-1, 2, 2, 2, 2, 2, -1},
	  {-1, 2, 2, 2, 2, 2, -1},
	};

	hls::Window<KS,KS,COEF_T> Sh;
	for (int r=0; r<KS; r++) for (int c=0; c<KS; c++) Sh.val[r][c] = coef_h[r][c];

	// point
	hls::Point_<INDEX_T> anchor;
	anchor.x=-1;
	anchor.y=-1;

	hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sh, anchor);

}


// -----------------------------------
// 	135 Degree Thin Line Filter
// -----------------------------------

template<typename IMG_T>
void thinLine_filter_135Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols ) {

	// 2D kernel for 135 degree lines
	const COEF_T coef_h[KS][KS]= {
	  { 2,   2, -1, -1, -1,  -1,  -1},
	  { 2,   2,  2, -1, -1,  -1,  -1},
	  {-1,   2,  2,  2, -1,  -1,  -1},
	  {-1,  -1,  2,  2,  2,  -1,  -1},
	  {-1,  -1, -1,  2,  2,   2,  -1},
	  {-1,  -1, -1, -1,  2,   2,   2},
	  {-1,  -1, -1, -1, -1,   2,   2}
	};

	hls::Window<KS,KS,COEF_T> Sh;
	for (int r=0; r<KS; r++) for (int c=0; c<KS; c++) Sh.val[r][c] = coef_h[r][c];

	// point
	hls::Point_<INDEX_T> anchor;
	anchor.x=-1;
	anchor.y=-1;

	hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sh, anchor);

}

// -----------------------------------
// 	135 Degree Thick Line Filter
// -----------------------------------

template<typename IMG_T>
void thickLine_filter_135Degrees(IMG_T& img_in, IMG_T& img_out, int rows, int cols ) {

	// 2D kernel for 135 degree lines
	const COEF_T coef_h[KS][KS]= {
	  { 2,   2,  2, -1, -1,  -1,  -1},
	  { 2,   2,  2,  2, -1,  -1,  -1},
	  { 2,   2,  2,  2,  2,  -1,  -1},
	  {-1,   2,  2,  2,  2,   2,  -1},
	  {-1,  -1,  2,  2,  2,   2,   2},
	  {-1,  -1, -1,  2,  2,   2,   2},
	  {-1,  -1, -1, -1,  2,   2,   2}
	};

	hls::Window<KS,KS,COEF_T> Sh;
	for (int r=0; r<KS; r++) for (int c=0; c<KS; c++) Sh.val[r][c] = coef_h[r][c];

	// point
	hls::Point_<INDEX_T> anchor;
	anchor.x=-1;
	anchor.y=-1;

	hls::Filter2D <hls::BORDER_CONSTANT> (img_in, img_out, Sh, anchor);

}

// -----------------------------------
//  AND Two Images
// -----------------------------------

template<typename IMG_T, typename PIXEL_T>
void andImages(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_out, int rows, int cols) {

	PIXEL_T pin0, pin1;
	PIXEL_T pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=1 max=1080
		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=1920
		#pragma HLS pipeline rewind

				   img_in0 >> pin0;
				   img_in1 >> pin1;

				   if(pin0.val[0]>230 && pin1.val[0] >230){
						pout = pin0;
				   }
				   else{
						pout = 0;
				   }

				   img_out << pout;
				}
		}

}

// -----------------------------------
// 	OR Two Images
// -----------------------------------

template<typename IMG_T, typename PIXEL_T>
void orImages(IMG_T& img_in0, IMG_T& img_in1, IMG_T& img_out, int rows, int cols) {

	PIXEL_T pin0, pin1;
	PIXEL_T pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=1 max=1080
		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=1920
		#pragma HLS pipeline rewind

				   img_in0 >> pin0;
				   img_in1 >> pin1;

				   if(pin0.val[0]>230  ){
						pout = pin0;
				   }
				   else if(pin1.val[0] >230){
						pout = pin1;
				   }
				   else{
						pout = 0;
				   }

				   img_out << pout;
				}
		}

}

// -----------------------------------
//  Clear Image Top Half
// -----------------------------------

template<typename IMG_T, typename PIXEL_T>
void clearImageTop(IMG_T& img_in0, IMG_T& img_out, int rows, int cols) {

	PIXEL_T pin0, pin1;
	PIXEL_T pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=1 max=1080
		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=1920
		#pragma HLS pipeline rewind

				   img_in0 >> pin0;


				   if(row > rows/2){
						pout = pin0;
				   }
				   else{
						pout = 0;
				   }

				   img_out << pout;
				}
		}

}

// -----------------------------------
// 	Threshold function for RGB image
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

template<typename IMG_T, typename PIXEL_T>
void replicate_by3(IMG_T& img_in, IMG_T& img_out0, IMG_T& img_out1, IMG_T& img_out2, int rows, int cols) {

	PIXEL_T pin;
	PIXEL_T pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=1 max=1080
		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=1920
		#pragma HLS pipeline rewind

				   img_in >> pin;

				   pout = pin;

				   img_out0 << pout;
				   img_out1 << pout;
				   img_out2 << pout;
				}
		}

}
