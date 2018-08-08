// -------------------------------------------------------------------------
//       DCU Vehicle counter IP
//
//  Author:       Cathal Garry
//  Description:  Custom IP for counting the number of
//                vehicles on a motorway
// ------------------------------------------------------------------------

#include "vehicleCount.h"

void vehicleCount(AXI_STREAM& stream_in, AXI_STREAM& stream_out, int& leftLaneCount, int& rightLaneCount, int& result)
{
    // ----------------------------------------
    //   directives
    // ----------------------------------------

    // creates AXI stream ports
    #pragma HLS INTERFACE axis register both port=stream_out
    #pragma HLS INTERFACE axis register both port=stream_in

    // creates AXIS registers
    #pragma HLS INTERFACE s_axilite port=result
    #pragma HLS INTERFACE s_axilite port=leftLaneCount
    #pragma HLS INTERFACE s_axilite port=rightLaneCount

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

    // RGB Frames
    RGB_IMAGE 	 imgRGBIn      		(rows, cols);
    RGB_IMAGE 	 imgEdges   		(rows, cols);
    RGB_IMAGE 	 imgOut      		(rows, cols);
    RGB_IMAGE 	 imgRGBOut          (rows, cols);

    // Grayscale Frames
    SINGLE_IMAGE 	 imgDuplicate0      (rows, cols);
    SINGLE_IMAGE 	 imgDuplicate1      (rows, cols);
    SINGLE_IMAGE 	 imgGray     		(rows, cols);
    SINGLE_IMAGE 	 imgDilate0  		(rows, cols);
    SINGLE_IMAGE 	 imgDilate1  		(rows, cols);
    SINGLE_IMAGE     imgThreshold       (rows, cols);

    // ----------------------------------------
    //  Stage 1:  Input Stream
    // ----------------------------------------

    hls::AXIvideo2Mat(stream_in, imgRGBIn);

    // ----------------------------------------
    //  Stage 2:  Edge Detection
    // ----------------------------------------

    edge_detect<RGB_IMAGE>(imgRGBIn, imgEdges);

    // -----------------------------------------
    //  Stage 2:  RGB to Grayscale
    // -----------------------------------------

    hls::CvtColor<HLS_RGB2GRAY>(imgEdges, imgGray);

    // ----------------------------------------
    //  Stage 3: Dilate
    // ----------------------------------------

    hls::Dilate(imgGray, imgDilate0);
    hls::Dilate(imgDilate0, imgDilate1);

    // ----------------------------------------
    //  Stage 3: Threshold
    // ----------------------------------------

    hls::Threshold(imgDilate1, imgThreshold, 80, 255, HLS_THRESH_BINARY );

    // -----------------------------------------
    //  Stage 5: GrayScale to RGB
    // -----------------------------------------

    hls::CvtColor<HLS_GRAY2RGB>(imgThreshold, imgRGBOut);

    // ----------------------------------------
    //  Stage 6: Calculate result
    // ----------------------------------------

    detectVehicleInLane<RGB_IMAGE,RGB_PIXEL>(imgRGBOut, imgOut, rows, cols, leftLaneCount,
			                                 rightLaneCount,result);

    // ----------------------------------------
    // output image
    // ----------------------------------------

    hls::Mat2AXIvideo(imgOut, stream_out);
}




template<typename IMG_T, typename PIXEL_T>
void detectVehicleInLane(
	IMG_T& img_in,
	IMG_T& img_out,
	int rows,
	int cols,
	int& leftLaneCount,
	int& rightLaneCount,
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

	// Lane monitors
	const int pixelLinesMonitored = 5;
	static unsigned int frameCount = 0;
	static unsigned int leftLane0Monitor[pixelLinesMonitored]={0,0,0,0,0};
	static unsigned int leftLane1Monitor[pixelLinesMonitored]={0,0,0,0,0};
	static unsigned int leftLane2Monitor[pixelLinesMonitored]={0,0,0,0,0};
	static unsigned int rightLane0Monitor[pixelLinesMonitored]={0,0,0,0,0};
	static unsigned int rightLane1Monitor[pixelLinesMonitored]={0,0,0,0,0};
	static unsigned int rightLane2Monitor[pixelLinesMonitored]={0,0,0,0,0};

	// ensures lanes counts are set
	// to zero on the first frame
    if(frameCount==0)
    {
    	leftLaneCount=0;
    	rightLaneCount=0;
    }

    // ------------------------------------
    //  Image Processing
    // ------------------------------------

	Row: for(int row = 0; row < rows; row++) {
		#pragma HLS LOOP_TRIPCOUNT min=1 max=720

		Col: for(int col = 0; col < cols; col++) {
			   #pragma HLS LOOP_TRIPCOUNT min=1 max=1280
			   #pragma HLS pipeline rewind

			   // input pixels
			   img_in >> pin0;

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
					   // Pass through debug image
					   pout.val[0] = pin0.val[0];
					   pout.val[1] = pin0.val[1];
					   pout.val[2] = pin0.val[2];
			   }

			   // output pixel
			   img_out << pout;
		}
	}

    // --------------------------------------------------
	// Updating Monitors
    // --------------------------------------------------

	// shift old value by one to the left
    for(int i=pixelLinesMonitored-1; i>=0; --i)
    {
        leftLane0Monitor[i] = leftLane0Monitor[i-1];
        leftLane1Monitor[i] = leftLane1Monitor[i-1];
        leftLane2Monitor[i] = leftLane2Monitor[i-1];
        rightLane0Monitor[i] = rightLane0Monitor[i-1];
        rightLane1Monitor[i] = rightLane1Monitor[i-1];
        rightLane2Monitor[i] = rightLane2Monitor[i-1];
    }

    // add new values
    leftLane0Monitor[0]= (leftLane0Cnt>=12) ? 1 : 0;
    leftLane1Monitor[0]= (leftLane1Cnt>=12) ? 1 : 0;
    leftLane2Monitor[0]= (leftLane2Cnt>=12) ? 1 : 0;
    rightLane0Monitor[0]= (rightLane0Cnt>=12) ? 1 : 0;
    rightLane1Monitor[0]= (rightLane1Cnt>=12) ? 1 : 0;
    rightLane2Monitor[0]= (rightLane2Cnt>=12) ? 1 : 0;


    // --------------------------------------------------
    //  Determine wich lanes have a vehicle in them
    //    - each lane is represented by one bit
    //    - set to one to incidate there is a vehicle
    // --------------------------------------------------

    if(leftLane0Cnt>=5)
    {
       // update result register
	   result=result+1;

	   // Determine if the count should be increased
	   bool increaseCount=false;
	   for(int x=0; x<pixelLinesMonitored ; x++)
	   {
		   if(x==0){
			   if(leftLane0Monitor[x]==1)
			   {
				   increaseCount=true;
			   }
		   }
		   else{
			   if(leftLane0Monitor[x]==1)
			   {
				   increaseCount=false;
			   }
		   }
	   }

	   // Increase count
	   if(increaseCount==true)
	   {
		   leftLaneCount=leftLaneCount+1;
	   }

    }
    if(leftLane1Cnt>=5)
    {
       // Update result register
	   result=result+2;


	   // Determine if the count should be increased
	   bool increaseCount=false;
	   for(int x=0; x<pixelLinesMonitored ; x++)
	   {
		   if(x==0){
			   if(leftLane1Monitor[x]==1)
			   {
				   increaseCount=true;
			   }
		   }
		   else{
			   if(leftLane1Monitor[x]==1)
			   {
				   increaseCount=false;
			   }
		   }
	   }

	   // Increase count
	   if(increaseCount==true)
	   {
		   leftLaneCount=leftLaneCount+1;
	   }

    }
    if(leftLane2Cnt>=5)
    {
       // Update result register
	   result=result+4;


	   // Determine if the count should be increased
	   bool increaseCount=false;
	   for(int x=0; x<pixelLinesMonitored ; x++)
	   {
		   if(x==0){
			   if(leftLane2Monitor[x]==1)
			   {
				   increaseCount=true;
			   }
		   }
		   else{
			   if(leftLane2Monitor[x]==1)
			   {
				   increaseCount=false;
			   }
		   }
	   }

	   // Increase count
	   if(increaseCount==true)
	   {
		   leftLaneCount=leftLaneCount+1;
	   }

    }
    if(rightLane0Cnt>=5)
    {
       // Update result register
	   result=result+8;


	   // Determine if the count should be increased
	   bool increaseCount=false;
	   for(int x=0; x<pixelLinesMonitored ; x++)
	   {
		   if(x==0){
			   if(rightLane0Monitor[x]==1)
			   {
				   increaseCount=true;
			   }
		   }
		   else{
			   if(rightLane0Monitor[x]==1)
			   {
				   increaseCount=false;
			   }
		   }
	   }

	   // Increase count
	   if(increaseCount==true)
	   {
		   rightLaneCount=rightLaneCount+1;
	   }

    }
    if(rightLane1Cnt>=5)
    {
       // Update result value
	   result=result+16;


	   // Determine if the count should be increased
	   bool increaseCount=false;
	   for(int x=0; x<pixelLinesMonitored ; x++)
	   {
		   if(x==0){
			   if(rightLane1Monitor[x]==1)
			   {
				   increaseCount=true;
			   }
		   }
		   else{
			   if(rightLane1Monitor[x]==1)
			   {
				   increaseCount=false;
			   }
		   }
	   }

	   // Increase count
	   if(increaseCount==true)
	   {
		   rightLaneCount=rightLaneCount+1;
	   }

    }
    if(rightLane2Cnt>=5)
    {
       // Update results value
	   result=result+32;


	   // Determine if the count should be increased
	   bool increaseCount=false;
	   for(int x=0; x<pixelLinesMonitored ; x++)
	   {
		   if(x==0){
			   if(rightLane2Monitor[x]==1)
			   {
				   increaseCount=true;
			   }
		   }
		   else{
			   if(rightLane2Monitor[x]==1)
			   {
				   increaseCount=false;
			   }
		   }
	   }

	   // Increase count
	   if(increaseCount==true)
	   {
		   rightLaneCount=rightLaneCount+1;
	   }
    }

    // ------------------------------------
    //  Increment frame counter
    // ------------------------------------
    frameCount++;
}

template<typename IMG_T>
void edge_detect(IMG_T& imgInput, IMG_T& imgOutput)
{
    // Image width and height
    int const rows = MAX_HEIGHT;
    int const cols = MAX_WIDTH;

    // local storage
    // Single Frame
    SINGLE_IMAGE imgGray(rows, cols);
    SINGLE_IMAGE imgDilate(rows, cols);
    SINGLE_IMAGE imgEdges0(rows, cols);
    SINGLE_IMAGE imgEdges1(rows, cols);
    SINGLE_IMAGE imgDuplicate0(rows, cols);
    SINGLE_IMAGE imgDuplicate1(rows, cols);
    SINGLE_IMAGE imgScale0(rows, cols);
    SINGLE_IMAGE imgScale1(rows, cols);
    SINGLE_IMAGE imgGrad(rows, cols);


	// Adding delay logic
	#pragma HLS stream depth=20000 variable=imgDuplicate0.data_stream
	#pragma HLS stream depth=20000 variable=imgDuplicate1.data_stream

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
