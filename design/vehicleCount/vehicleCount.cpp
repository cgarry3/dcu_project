#include "vehicleCount.h"

void vehicleCount(stream_t& stream_in, stream_t& stream_out, int& result)
{
	// ----------------------------------------
	//   directives
	// ----------------------------------------

	#pragma HLS INTERFACE axis register both port=stream_out
	#pragma HLS INTERFACE axis register both port=stream_in
	#pragma HLS INTERFACE s_axilite port=result
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

    rgb_img_t imgRGB(rows, cols);
    rgb_img_t imgGray(rows, cols);
    rgb_img_t imgDilate0(rows, cols);
    rgb_img_t imgDilate1(rows, cols);
    rgb_img_t imgDilate2(rows, cols);
    rgb_img_t imgEdges0(rows, cols);
    rgb_img_t imgEdges1(rows, cols);
    rgb_img_t imgMerge(rows, cols);
    rgb_img_t imgOut(rows, cols);
    single_img_t imgSplitCh0(rows, cols);
    single_img_t imgSplitCh1(rows, cols);
    single_img_t imgSplitCh2(rows, cols);
    single_img_t imgThresholdCh0(rows, cols);
    single_img_t imgThresholdCh1(rows, cols);
    single_img_t imgThresholdCh2(rows, cols);


    // ----------------------------------------
    //  Stage 1:  Remove colour
    // ----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgRGB);
    hls::CvtColor<HLS_RGB2GRAY>(imgRGB, imgGray);



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

    detectVehicleInLane<rgb_img_t,rgb_pixel_t>(imgMerge, imgOut, rows, cols, result);

    // ----------------------------------------
    // output image
    // ----------------------------------------

    hls::Mat2AXIvideo(imgOut, stream_out);
}




template<typename IMG_T, typename PIXEL_T>
void detectVehicleInLane(
	IMG_T& img_in0,
	IMG_T& img_out,
	int rows,
	int cols,
	int& result)
{

	PIXEL_T pin0;
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

			   // input pixel
			   img_in0 >> pin0;

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
				   // pass value through
				   pout.val[0] = pin0.val[0];
				   pout.val[1] = pin0.val[1];
				   pout.val[2] = pin0.val[2];
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
