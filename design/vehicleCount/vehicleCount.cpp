// -------------------------------------------------------------------------
//       DCU Vehicle counter IP
//
//  Author:       Cathal Garry
//  Description:  Custom IP for counting the number of
//                vehicles on a motorway
// ------------------------------------------------------------------------

#include "vehicleCount.h"

void vehicleCount(AXI_STREAM& stream_in, AXI_STREAM& stream_out, int& debug, int& result)
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

    // Region of interests constant values
    const int roiWidth  =    ROIWIDTH;
    const int roiHeight =    ROIHEIGHT;
    const int roiLeftLaneY  = ROILeftLaneY;
    const int roiLeftLaneX  = ROILeftLaneX;
    const int roiStrideLeft = ROIStrideLeft;
    const int roiRightLaneY  = ROIRightLaneY;
    const int roiRightLaneX  = ROIRightLaneX;
    const int roiStrideRight = ROIStrideRight;
    const int numLeftLanes   = NUMLEFTLANES;
    const int numRightLanes  = NUMRIGHTLANES;

	// ----------------------------------------
	//   Local Storage
	// ----------------------------------------

    RGB_IMAGE 	 imgRGB      		(rows, cols);
    RGB_IMAGE 	 imgDuplicate0      (rows, cols);
    RGB_IMAGE 	 imgDuplicate1      (rows, cols);
    RGB_IMAGE 	 imgGray     		(rows, cols);
    RGB_IMAGE 	 imgDilate0  		(rows, cols);
    RGB_IMAGE 	 imgDilate1  		(rows, cols);
    RGB_IMAGE 	 imgDilate2  		(rows, cols);
    RGB_IMAGE 	 imgEdges0   		(rows, cols);
    RGB_IMAGE 	 imgEdges1   		(rows, cols);
    RGB_IMAGE 	 imgMerge    		(rows, cols);
    RGB_IMAGE 	 imgOut      		(rows, cols);
    SINGLE_IMAGE imgSplitCh0 		(rows, cols);
    SINGLE_IMAGE imgSplitCh1 		(rows, cols);
    SINGLE_IMAGE imgSplitCh2 		(rows, cols);
    SINGLE_IMAGE imgThresholdCh0    (rows, cols);
    SINGLE_IMAGE imgThresholdCh1    (rows, cols);
    SINGLE_IMAGE imgThresholdCh2    (rows, cols);


    // ----------------------------------------
    //  Stage 1:  Remove colour
    // ----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgRGB);
    replicate(imgRGB, imgDuplicate0, imgDuplicate1, rows, cols);
    hls::CvtColor<HLS_RGB2GRAY>(imgDuplicate0, imgGray);



    // ----------------------------------------
    //  Stage 2:  Edge Detection
    // ----------------------------------------

    hls::Sobel<1,0,3>(imgGray, imgEdges0);
    hls::CvtColor<HLS_GRAY2RGB>(imgEdges0, imgEdges1);

    // ----------------------------------------
    //  Stage 3: Dilate
    // ----------------------------------------

    hls::Dilate(imgEdges1, imgDilate0);
    hls::Dilate(imgDilate0, imgDilate1);

    // ----------------------------------------
    //  Stage 3: Threshold
    // ----------------------------------------

    // split three data channels
    hls::Split(imgDilate1, imgSplitCh0, imgSplitCh1, imgSplitCh2);

    // Threshold
    hls::Threshold(imgSplitCh0, imgThresholdCh0, 30, 254, HLS_THRESH_BINARY );
    hls::Threshold(imgSplitCh1, imgThresholdCh1, 30, 254, HLS_THRESH_BINARY );
    hls::Threshold(imgSplitCh2, imgThresholdCh2, 30, 254, HLS_THRESH_BINARY );

    hls::Merge( imgThresholdCh0, imgThresholdCh1, imgThresholdCh2, imgMerge );

    // ----------------------------------------
    //  Stage 4: Calculate result
    // ----------------------------------------

    detectVehicleInLane<RGB_IMAGE,RGB_PIXEL>(imgMerge, imgDuplicate1, imgOut, rows, cols, debug,result);

    // ----------------------------------------
    // output image
    // ----------------------------------------

    hls::Mat2AXIvideo(imgOut, stream_out);
}




template<typename IMG_T, typename PIXEL_T>
void detectVehicleInLane(
	IMG_T& img_in0,
	IMG_T& img_in1,
	IMG_T& img_out,
	int rows,
	int cols,
	int& debug,
	int& result)
{

	PIXEL_T pin0;
	PIXEL_T pin1;
	PIXEL_T pout;

	// set result to 0 by default
	result=0;

	// lane counts
	int leftLane0Cnt=0;
	int leftLane1Cnt=0;
	int leftLane2Cnt=0;
	int rightLane0Cnt=0;
	int rightLane1Cnt=0;
	int rightLane2Cnt=0;

	Row: for(int row = 0; row < rows; row++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=720

		Col: for(int col = 0; col < cols; col++) {
			   #pragma HLS LOOP_TRIPCOUNT min=1 max=1280
			   #pragma HLS pipeline rewind

			   // input pixels
			   img_in0 >> pin0;
			   img_in1 >> pin1;

			   // --------------------------------------------------
			   //  Left hand side of the motorway
			   // --------------------------------------------------

               // left lane0
			   if((col>ROILeftLaneX && col<(ROILeftLaneX+ROIWIDTH)) && row==ROILeftLaneY){
				   //set pixel red
				   pout.val[0] = 0;      // B
				   pout.val[1] = 0;      // G
				   pout.val[2] = 255;    // R

				   if (pin0.val[0]>100 | pin0.val[1]>100 | pin0.val[2]>100)
				   {
					   leftLane0Cnt=leftLane0Cnt+1;
				   }
			   }
               // left lane1
			   else if((col>(ROILeftLaneX+ROIStrideLeft) && col<(ROILeftLaneX+ROIWIDTH+ROIStrideLeft)) && row==ROILeftLaneY){
				   //set pixel red
				   pout.val[0] = 0;      // B
				   pout.val[1] = 0;      // G
				   pout.val[2] = 255;    // R

				   if (pin0.val[0]>100 | pin0.val[1]>100 | pin0.val[2]>100)
				   {
					   leftLane1Cnt=leftLane1Cnt+1;
				   }
			   }
               // left lane2
			   else if((col>(ROILeftLaneX+(ROIStrideLeft*2)) && col<(ROILeftLaneX+ROIWIDTH+(ROIStrideLeft*2))) && row==ROILeftLaneY){
				   //set pixel red
				   pout.val[0] = 0;      // B
				   pout.val[1] = 0;      // G
				   pout.val[2] = 255;    // R

				   if (pin0.val[0]>100 | pin0.val[1]>100 | pin0.val[2]>100)
				   {
					   leftLane2Cnt=leftLane2Cnt+1;
				   }
			   }

			   // --------------------------------------------------
			   //  Right hand side of the motorway
			   // --------------------------------------------------

               // right lane0
			   else if((col>ROIRightLaneX && col<(ROIRightLaneX+ROIWIDTH)) && row==ROIRightLaneY){
				   //set pixel red
				   pout.val[0] = 0;      // B
				   pout.val[1] = 0;      // G
				   pout.val[2] = 255;    // R

				   if (pin0.val[0]>100 | pin0.val[1]>100 | pin0.val[2]>100)
				   {
					   rightLane0Cnt=rightLane0Cnt+1;
				   }
			   }
               // right lane1
			   else if((col>(ROIRightLaneX+ROIStrideRight) && col<(ROIRightLaneX+ROIWIDTH+ROIStrideRight)) && row==ROIRightLaneY){
				   //set pixel red
				   pout.val[0] = 0;      // B
				   pout.val[1] = 0;      // G
				   pout.val[2] = 255;    // R

				   if (pin0.val[0]>100 | pin0.val[1]>100 | pin0.val[2]>100)
				   {
					   rightLane1Cnt=rightLane1Cnt+1;
				   }
			   }
               // right lane2
			   else if((col>(ROIRightLaneX+(ROIStrideRight*2)) && col<(ROIRightLaneX+ROIWIDTH+(ROIStrideRight*2))) && row==ROIRightLaneY){
				   //set pixel red
				   pout.val[0] = 0;      // B
				   pout.val[1] = 0;      // G
				   pout.val[2] = 255;    // R

				   if (pin0.val[0]>100 | pin0.val[1]>100 | pin0.val[2]>100)
				   {
					   rightLane2Cnt=rightLane2Cnt+1;
				   }
			   }

			   // --------------------------------------------------
			   //  Bypass for Data
			   // --------------------------------------------------

			   else{
				   if(debug==0)
				   {
					   // Pass through orginal image
					   pout.val[0] = pin1.val[0];
					   pout.val[1] = pin1.val[1];
					   pout.val[2] = pin1.val[2];
				   }
				   else
				   {
					   // Pass through debug image
					   pout.val[0] = pin0.val[0];
					   pout.val[1] = pin0.val[1];
					   pout.val[2] = pin0.val[2];
				   }
			   }

			   // output pixel
			   img_out << pout;
		}
	}

    // --------------------------------------------------
    //  Determine wich lanes have a vehicle in them
    //    - each lane is represented by one bit
    //    - set to one to incidate there is a vehicle
    // --------------------------------------------------

    if(leftLane0Cnt>=12)
    {
	   result=result+1;
    }
    if(leftLane1Cnt>=12)
    {
	   result=result+2;
    }
    if(leftLane2Cnt>=12)
    {
	   result=result+4;
    }
    if(rightLane0Cnt>=12)
    {
	   result=result+8;
    }
    if(rightLane1Cnt>=12)
    {
	   result=result+16;
    }
    if(rightLane2Cnt>=12)
    {
	   result=result+32;
    }
}


// -----------------------------------
// replicate stream
// -----------------------------------
void replicate(RGB_IMAGE& img_in, RGB_IMAGE& img_out0, RGB_IMAGE& img_out1, int rows, int cols)
{

	RGB_PIXEL pin;
	RGB_PIXEL pout;

	L_row: for(int row = 0; row < rows; row++) {
	#pragma HLS LOOP_TRIPCOUNT min=720 max=1080

		L_col: for(int col = 0; col < cols; col++) {
		#pragma HLS LOOP_TRIPCOUNT min=1280 max=1920
		#pragma HLS pipeline rewind

				   img_in >> pin;

				   pout = pin;

				   img_out0 << pout;
				   img_out1 << pout;
				}
		}

}
