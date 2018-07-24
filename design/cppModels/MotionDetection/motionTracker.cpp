// =======================================================
// Author:       Cathal Garry
// Email:        cathal.garry3@mail.dcu.ie
// Date:         10 July 2018
// Description:  This is program tracks the motion of 
//               vehicles from one image to another.
// ========================================================

// --------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>

#include<iostream>
#include <sstream>
#include <string>
#include <time.h>



// --------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------

#define REDUCED_GRAN_BOX_SIZE 5
#define ROWS 720
#define COLS 1280
#define MOTIONBOXSIZE 40


// --------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------

const cv::Scalar COLOUR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar COLOUR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar COLOUR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar COLOUR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar COLOUR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar COLOUR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

// Image width and height
const int  rows = ROWS;
const int  cols = COLS;


// constant values
const int motionBoxSize     = MOTIONBOXSIZE;
const int numOfMotionBoxes  = (rows*cols)/(motionBoxSize*motionBoxSize);
const int numOfTrackingInts = (numOfMotionBoxes/32) + 1;


// function defines
const int ROIWIDTH  =    40;
const int ROIHEIGHT =    25;

const int ROILeftLaneY  = 396;
const int ROILeftLaneX  = 330;
const int ROIStrideLeft = 90;

const int ROIRightLaneY  = 396;
const int ROIRightLaneX  = 720;
const int ROIStrideRight = 80;

const int NUMLEFTLANES  = 3;
const int NUMRIGHTLANES = 3;


// --------------------------------------------------------------------
// Function Declarations
// --------------------------------------------------------------------
void checkIfvehiclesCrossedOnLine(cv::Mat &imgFrame, int &result);
void addCountToImage(int leftLaneVehicleCount,int rightLaneVehicleCount, cv::Mat &imgFrame);
void addROIToImage(cv::Mat &imgFrame, cv::Rect rects[], int rectsSize);
void motionDection(cv::Mat &imgIn, cv::Mat &imgOut, int rows, int cols);

void thinLine_filter_45Degrees(cv::Mat& img_in, cv::Mat& img_out);
void thickLine_filter_45Degrees(cv::Mat& img_in, cv::Mat& img_out);
void thinLine_filter_90Degrees(cv::Mat& img_in, cv::Mat& img_out);
void thickLine_filter_90Degrees(cv::Mat& img_in, cv::Mat& img_out);
void thinLine_filter_135Degrees(cv::Mat& img_in, cv::Mat& img_out);
void thickLine_filter_135Degrees(cv::Mat& img_in, cv::Mat& img_out);

// --------------------------------------------------------------------
// Main Function
// --------------------------------------------------------------------

int main(void) {

    // ---------------------------------------------------------------------
    //  Variables
    // ---------------------------------------------------------------------

    // ------------------------------
    //  Control Variables
    // ------------------------------


    // used to check for esc key
    // if pressed the program closes
    char chCheckForEscKey = 0;

    // frame count
    int frameCount = 1;


    // motorway tracker for all lanes
    motorwayTraffic M6LeftLane0;
    motorwayTraffic M6LeftLane1;
    motorwayTraffic M6LeftLane2;
    motorwayTraffic M6RightLane0;
    motorwayTraffic M6Rightane1;
    motorwayTraffic M6RightLane2;


    // ------------------------------
    //  Video and Image Variables
    // ------------------------------

    // traffic video
    cv::VideoCapture capVideo;

    // Image variables
    cv::Mat inpuFrame1;



    // Opening Video file
    capVideo.open("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6 Motorway Traffic.mp4");


    // Error message if the video file wasn't opened correctly
    if (!capVideo.isOpened()) {
        std::cout << "error reading video file" << std::endl << std::endl;
        return(0);
    }

    // Error Message if there is less than two frames in the video
    if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 1) {
        std::cout << "error: video file has no frames left";
        //_getch();                   // it may be necessary to change or remove this line if not using Windows
        return(0);
    }

    // Loading the two images
    capVideo.read(inpuFrame1);


    // ------------------------------
    //  Vehicle Count Lines
    // ------------------------------


    // States specific to the video
    int countLinePosition = (int)std::round((double)inpuFrame1.rows * 0.55);
    int leftLane0 = 330;
    int leftLane1 = 420;
    int leftLane2 = 510;
    int rightLane0 = 720;
    int rightLane1 = 800;
    int rightLane2 = 880;


    // regions of interest width and height
    int width  = 40;
    int height = 25;

    // left lane 0
    cv::Point ROIL0P0 = {leftLane0,countLinePosition};
    cv::Point ROIL0P1 = {leftLane0+width,countLinePosition};
    cv::Point ROIL0P2 = {leftLane0,countLinePosition+height};
    cv::Point ROIL0P3 = {leftLane0+width,countLinePosition+height};
    std::vector<cv::Point> points0;
    points0.push_back(ROIL0P0);
    points0.push_back(ROIL0P1);
    points0.push_back(ROIL0P2);
    points0.push_back(ROIL0P3);
    cv::Rect leftLane0Rect =  cv::boundingRect(points0);

    // left lane 1
    cv::Point ROIL1P0 = {leftLane1,countLinePosition};
    cv::Point ROIL1P1 = {leftLane1+width,countLinePosition};
    cv::Point ROIL1P2 = {leftLane1,countLinePosition+height};
    cv::Point ROIL1P3 = {leftLane1+width,countLinePosition+height};
    std::vector<cv::Point> points1;
    points1.push_back(ROIL1P0);
    points1.push_back(ROIL1P1);
    points1.push_back(ROIL1P2);
    points1.push_back(ROIL1P3);
    cv::Rect leftLane1Rect =  cv::boundingRect(points1);

    // left lane 2
    cv::Point ROIL2P0 = {leftLane2,countLinePosition};
    cv::Point ROIL2P1 = {leftLane2+width,countLinePosition};
    cv::Point ROIL2P2 = {leftLane2,countLinePosition+height};
    cv::Point ROIL2P3 = {leftLane2+width,countLinePosition+height};
    std::vector<cv::Point> points2;
    points2.push_back(ROIL2P0);
    points2.push_back(ROIL2P1);
    points2.push_back(ROIL2P2);
    points2.push_back(ROIL2P3);
    cv::Rect leftLane2Rect =  cv::boundingRect(points2);

    // right lane 0
    cv::Point ROIL3P0 = {rightLane0,countLinePosition};
    cv::Point ROIL3P1 = {rightLane0+width,countLinePosition};
    cv::Point ROIL3P2 = {rightLane0,countLinePosition+height};
    cv::Point ROIL3P3 = {rightLane0+width,countLinePosition+height};
    std::vector<cv::Point> points3;
    points3.push_back(ROIL3P0);
    points3.push_back(ROIL3P1);
    points3.push_back(ROIL3P2);
    points3.push_back(ROIL3P3);
    cv::Rect rightLane0Rect =  cv::boundingRect(points3);

    // right lane 1
    cv::Point ROIL4P0 = {rightLane1,countLinePosition};
    cv::Point ROIL4P1 = {rightLane1+width,countLinePosition};
    cv::Point ROIL4P2 = {rightLane1,countLinePosition+height};
    cv::Point ROIL4P3 = {rightLane1+width,countLinePosition+height};
    std::vector<cv::Point> points4;
    points4.push_back(ROIL4P0);
    points4.push_back(ROIL4P1);
    points4.push_back(ROIL4P2);
    points4.push_back(ROIL4P3);
    cv::Rect rightLane1Rect =  cv::boundingRect(points4);

    // right lane 2
    cv::Point ROIL5P0 = {rightLane2,countLinePosition};
    cv::Point ROIL5P1 = {rightLane2+width,countLinePosition};
    cv::Point ROIL5P2 = {rightLane2,countLinePosition+height};
    cv::Point ROIL5P3 = {rightLane2+width,countLinePosition+height};
    std::vector<cv::Point> points5;
    points5.push_back(ROIL5P0);
    points5.push_back(ROIL5P1);
    points5.push_back(ROIL5P2);
    points5.push_back(ROIL5P3);
    cv::Rect rightLane2Rect =  cv::boundingRect(points5);

    // array of ROIs
    int rectsSize = 6;
    cv::Rect rects[] = {leftLane0Rect, leftLane1Rect, leftLane2Rect, rightLane0Rect, rightLane1Rect, rightLane2Rect};



    // ---------------------------------------------------------------------
    //  Program Execution
    // ---------------------------------------------------------------------

    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        cv::Mat imgFrame1Copy = inpuFrame1.clone();
	    
        // -------------------------------------------------------
        //  Stage 4:  Update for next iteration
        // -------------------------------------------------------

	// default result
        int result=0;

        // motion detection
        motionDection(imgFrame1Copy, imgFrame1Copy,  rows, cols);

        // check for cars in each lane(needs to update for motion detection)
        //checkIfvehiclesCrossedOnLine(imgFrame1Copy, result);

        if((result&0x1)==1)
        {
        	// if its been 5 frames since the line was crossed increase count
        	// otherwise assume it is the same car
			if(M6LeftLane0.getNumberOfFramesSinceLineCrossed()>10)
			{
				M6LeftLane0.setNumberOfVehiclesPassed(M6LeftLane0.getNumberOfVehiclesPassed()+1);
	        	// frames back to 0
				M6LeftLane0.setNumberOfFramesSinceLineCrossed(0);
			}
        }
        else
        {
        	// if no car is seen increase the number of frames since
        	// the line was crossed
        	M6LeftLane0.setNumberOfFramesSinceLineCrossed(M6LeftLane0.getNumberOfFramesSinceLineCrossed()+1);
        }

        // right lane
        if((result&0x8)==8)
        {
        	// if its been 5 frames since the line was crossed increase count
        	// otherwise assume it is the same car
			if(M6RightLane0.getNumberOfFramesSinceLineCrossed()>10)
			{
				M6RightLane0.setNumberOfVehiclesPassed(M6RightLane0.getNumberOfVehiclesPassed()+1);
	        	// frames back to 0
				M6RightLane0.setNumberOfFramesSinceLineCrossed(0);
			}
        }
        else
        {
        	// if no car is seen increase the number of frames since
        	// the line was crossed
        	M6RightLane0.setNumberOfFramesSinceLineCrossed(M6RightLane0.getNumberOfFramesSinceLineCrossed()+1);
        }

        // show count on image
        int leftCount = M6LeftLane0.getNumberOfVehiclesPassed();
        int rightCount = M6RightLane0.getNumberOfVehiclesPassed();
        addCountToImage( leftCount , rightCount, imgFrame1Copy);

        cv::imshow("Count values", imgFrame1Copy);

        // show regions of interest
        addROIToImage(imgFrame1Copy, rects, rectsSize);

        cv::imshow("ROI of Interests", imgFrame1Copy);



        // Check for the end of the video
        if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
            capVideo.read(inpuFrame1);
        } else {
            std::cout << "end of video\n";
            break;
        }

	// update frame count
        frameCount++;
	    
	// Wait for ESC key
        chCheckForEscKey = cv::waitKey(1);

    }
    return(0);
}

// --------------------------------------------------------------------------------------------
// Check if vehicle crossed either count lines by just using the image
// --------------------------------------------------------------------------------------------

void checkIfvehiclesCrossedOnLine(cv::Mat &imgFrame, int &result){


    // -------------------------------------------------------
    //  Stage 1: Determine if car has passed a line in an image
    // -------------------------------------------------------

    // clear current value
    result = 0;


    // left lanes
    for(int j=0; j<NUMLEFTLANES; j++)
    {
    	// X + Y for ROI for lane
    	int x = ROILeftLaneX + (ROIStrideLeft*j);
    	int y = ROILeftLaneY;

    	// count for number white pixels
    	int numOfWhitePixels=0;

		for(int i=0; i<ROIWIDTH/2; i++)
		{
				cv::Scalar colour = imgFrame.at<uchar>(y+ROIHEIGHT,x+i);
				if(colour.val[0]==254)
				{
					numOfWhitePixels++;
				}
		}


    	if(numOfWhitePixels>8)
    	{
    		result = result + (1 << j);
    		std::cout << "car found !!!" << std::endl;
    	}
    }

    // right lanes
    for(int j=0; j<NUMRIGHTLANES; j++)
    {
    	// X + Y for ROI for lane
    	int x = ROIRightLaneX + (ROIStrideLeft*j);
    	int y = ROIRightLaneY;

    	// count for number white pixels
    	int numOfWhitePixels=0;

		for(int i=0; i<ROIWIDTH/2; i++)
		{
				cv::Scalar colour = imgFrame.at<uchar>(y+ROIHEIGHT,x+i);
				if(colour.val[0]==254)
				{
					numOfWhitePixels++;
				}
		}


    	if(numOfWhitePixels>8)
    	{
    		result = result + (8 << j);
    		std::cout << "car found !!!" << std::endl;
    	}
    }

	// ------------------------------------------
	// End Execution time
	// ------------------------------------------

	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

}


// --------------------------------------------------------------------------------------------
// Add Count To image
// --------------------------------------------------------------------------------------------

void addCountToImage(int leftLaneVehicleCount,int rightLaneVehicleCount, cv::Mat &imgFrame) {

    // -------------------------------------------
    //  Adding Left Lane Vehicle Count
    // -------------------------------------------

    int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
    double dblFontScale = (imgFrame.rows * imgFrame.cols) / 300000.0;
    int intFontThickness = (int)std::round(dblFontScale * 1.5);

    cv::Size textSize = cv::getTextSize(std::to_string(leftLaneVehicleCount), intFontFace, dblFontScale, intFontThickness, 0);

    cv::Point ptTextBottomLeftPosition;

    ptTextBottomLeftPosition.x = 50;//(int)((double)textSize.width * 1.25);
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

    cv::putText(imgFrame, std::to_string(leftLaneVehicleCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, COLOUR_GREEN, intFontThickness);

   // -------------------------------------------
   //  Adding Left Lane Vehicle Count
   // -------------------------------------------

    textSize = cv::getTextSize(std::to_string(leftLaneVehicleCount), intFontFace, dblFontScale, intFontThickness, 0);

    ptTextBottomLeftPosition.x = imgFrame.cols - (int)((double)textSize.width * 1.25);
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

    cv::putText(imgFrame, std::to_string(rightLaneVehicleCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, COLOUR_GREEN, intFontThickness);



}

// --------------------------------------------------------------------------------------------
// Add Info To image
// --------------------------------------------------------------------------------------------

void addROIToImage(cv::Mat &imgFrame, cv::Rect rects[], int rectsSize) {

	// -------------------------------------------
	// Adding regular box to Vehicles on image
	// -------------------------------------------

	for (int i = 0; i < rectsSize; i++) {

		// add tracking box
	    cv::rectangle(imgFrame, rects[i], COLOUR_RED, 2);

	    // add tracking number
	    int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
	    double fontScale = 2;
	    int thickness = 3;
	    int baseline=0;
	    cv::Size textSize = cv::getTextSize(std::to_string(i), fontFace,
					fontScale, thickness, &baseline);
	    cv::Point textOrg((rects[i].width - textSize.width)/2,
			     (rects[i].height + textSize.height)/2);

	    // add text
	    cv::putText(imgFrame, std::to_string(i), textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness, 8);
	}


}

// --------------------------------------------------------------------------------------------
//  Motion Detection
// --------------------------------------------------------------------------------------------

void motionDection(cv::Mat &imgIn, cv::Mat &imgOut, int rows, int cols)
{
    // variables for tracking motion
    static unsigned short int motionDetectPrevResult [numOfMotionBoxes];
    static unsigned short int motionDetectPresResult [numOfMotionBoxes];
    static unsigned int trackingInts                 [numOfTrackingInts];
    static unsigned int frameCnt=1;

    // image variables
    cv::Mat imgGray;
    cv::Mat imgDilate0;
    cv::Mat imgDilate1;
    cv::Mat imgEdges;
    cv::Mat imgThreshold;

    // ------------------------------------------
    // Start Execution time
    // ------------------------------------------

    clock_t tStart = clock();


    // -------------------------------------------------------
    //  Stage 1:  Turn Gray
    // -------------------------------------------------------

    cv::cvtColor( imgIn, imgGray, cv::COLOR_BGR2GRAY );

    // -------------------------------------------------------
    //  Stage 2:  Reduce granularity
    // -------------------------------------------------------

    cv::Mat reducedGranularity = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(REDUCED_GRAN_BOX_SIZE, REDUCED_GRAN_BOX_SIZE));

    // Dilating and erode the images
    cv::dilate(imgGray, imgDilate0, reducedGranularity);

    // -------------------------------------------------------
    //  Stage 3:  Create image of edge features
    // -------------------------------------------------------

    // sobel hyperparameters
    cv::Sobel(imgDilate0,imgEdges, CV_8U, 1, 0);


    // -------------------------------------------------------
    //  Stage 3:  Set all highed values to all white
    // -------------------------------------------------------

    cv::threshold(imgEdges, imgThreshold, 200, 254.0, CV_THRESH_BINARY);

    // -------------------------------------------------------
    //  Stage 4: Line Filters
    // -------------------------------------------------------

    // Split frame for parellal processing
    cv::Mat imgDiplicate0 = imgThreshold.clone();
    cv::Mat imgDiplicate1 = imgThreshold.clone();
    cv::Mat imgDiplicate2 = imgThreshold.clone();

    cv::Mat imgLineThin45;
    cv::Mat imgThresLine45Thin;
    cv::Mat imgLineThick45;
    cv::Mat imgThresLine45Thick;
    cv::Mat imgLineThin90;
    cv::Mat imgThresLine90Thin;
    cv::Mat imgLineThick90;
    cv::Mat imgThresLine90Thick;
    cv::Mat imgLineThin135;
    cv::Mat imgThresLine135Thin;
    cv::Mat imgLineThick135;
    cv::Mat imgThresLine135Thick;
    cv::Mat imgThres;


    // 45 degrees line filter
    thinLine_filter_45Degrees(imgDiplicate0, imgLineThin45);
    cv::threshold(imgLineThin45, imgThresLine45Thin, 20, 2, CV_THRESH_BINARY);
    thickLine_filter_45Degrees(imgThresLine45Thin, imgLineThick45);
    cv::threshold(imgLineThick45, imgThresLine45Thick, 20, 255, CV_THRESH_BINARY);

    // 90 degrees line filter
    thinLine_filter_90Degrees(imgDiplicate1, imgLineThin90);
    cv::threshold(imgLineThin90, imgThresLine90Thin, 20, 2, CV_THRESH_BINARY);
    thickLine_filter_90Degrees(imgThresLine90Thin, imgLineThick90);
    cv::threshold(imgLineThick90, imgThresLine90Thick, 20, 255, CV_THRESH_BINARY);

    // 135 degrees line filter
    thinLine_filter_135Degrees(imgDiplicate2, imgLineThin135);
    cv::threshold(imgLineThin135, imgThresLine135Thin, 20, 2, CV_THRESH_BINARY);
    thickLine_filter_135Degrees(imgThresLine135Thin, imgLineThick135);
    cv::threshold(imgLineThick135, imgThresLine135Thick, 20, 255, CV_THRESH_BINARY);

    // OR together all three images
    cv::Mat orImages = imgThresLine90Thick + imgThresLine135Thick + imgThresLine45Thick;

    // clear the top half of the image
    cv::Rect blankRoi(0, 0, orImages.size().width, orImages.size().height / 2);
    orImages(blankRoi).setTo(cv::Scalar(0));
    //imgThres = orImages.clone();

    cv::threshold(orImages, imgThres, 200, 255, CV_THRESH_BINARY);

    // show image
    cv::imshow("Line Filter Image", orImages);

    // copy present value to previous array of values
    for(int i=0; i<numOfMotionBoxes; i++){
	   motionDetectPrevResult[i] = motionDetectPresResult[i];
	   // clear old results
	   motionDetectPresResult[i] = 0;
    }

    // -------------------------------------------------------
    //  Stage 5: Optical Flow
    // -------------------------------------------------------

	for(int row = 0; row < rows; row++) {
		for(int col = 0; col < cols; col++) {
			       
			       // ------------------------------------
			       //  Stage 0: Input pixel
			       // ------------------------------------

			       uchar processedPixel  = orImages.at<uchar>(row, col);
			       int processedPixelValue  = (int)processedPixel;
			       cv::Vec3b pixelIn0 = imgIn.at<cv::Vec3b>(row, col);

			       // ------------------------------------
			       //  Stage 1: Current Motion Box and tracker number
			       // ------------------------------------


			       int motionBoxNum = (col/motionBoxSize) + ((row/motionBoxSize)*(cols/motionBoxSize));
		               int trackerNum   = (motionBoxNum/32);

			       // ------------------------------------
			       //   Stage 2: Set motion box to red
			       // ------------------------------------


				   int isSet = (trackingInts[trackerNum] >> (motionBoxNum%32)) & 0x1;

				   // see if motion box is set
				   if(isSet==0x1)
				   {
					   // set red
					   pixelIn0[0] = 0;
					   pixelIn0[1] = 0;
				   }



				   // ------------------------------------
				   //   Stage 3: Motion Detection
				   // ------------------------------------

		                   if(((col%motionBoxSize)==motionBoxSize-1) && ((row%motionBoxSize)==motionBoxSize-1)){
					   // add pixel value to count
					   if(processedPixelValue>200){
						   motionDetectPresResult[motionBoxNum] = motionDetectPresResult[motionBoxNum] + 1;
					   }

					   int SAD = abs(motionDetectPrevResult[motionBoxNum] - motionDetectPresResult[motionBoxNum]);

					   if(SAD>20)
					   {
						   trackingInts[trackerNum] = trackingInts[trackerNum] | (1 << (motionBoxNum%32));
					   }
					   else{
						   if(trackingInts[trackerNum]!=0){
						   	trackingInts[trackerNum] = trackingInts[trackerNum] & ~(1 << (motionBoxNum%32));
						   }
					   }
				   }
				   // Any other pixel
				   else if(processedPixelValue>200)
				   {

					   motionDetectPresResult[motionBoxNum] = motionDetectPresResult[motionBoxNum] + 1;


				   }


				   // ------------------------------------
				   //  Stage 4: Output pixel
				   // ------------------------------------

				   imgOut.at<cv::Vec3b>(row, col) = pixelIn0;
		}

	// increment frame count
	frameCnt++;

	// ------------------------------------------
	// End Execution time
	// ------------------------------------------

	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
}


// -----------------------------------
// 	45 Degree Thin Line Filter
// -----------------------------------

void thinLine_filter_45Degrees(cv::Mat& img_in, cv::Mat&  img_out) {

	// 2D kernel for 45 degree lines
	float kdata[]= {
	  -1, -1, -1, -1, -1,  2,  2,
	  -1, -1, -1, -1,  2,  2,  2,
	  -1, -1, -1,  2,  2,  2, -1,
	  -1, -1,  2,  2,  2, -1, -1,
	  -1,  2,  2,  2, -1, -1, -1,
	   2,  2,  2, -1, -1, -1, -1,
	   2,  2, -1, -1, -1, -1, -1
	};


	cv::Mat kernel(7,7,CV_32F, kdata);

	// 2D filter
	cv::filter2D(img_in, img_out, CV_32F, kernel);

}

// -----------------------------------
// 	45 Degree Thick Line Filter
// -----------------------------------

void thickLine_filter_45Degrees(cv::Mat& img_in, cv::Mat&  img_out ) {

	// 2D kernel for 45 degree lines
	float kdata[]= {
	  -1, -1, -1, -1,  2,  2,  2,
	  -1, -1, -1,  2,  2,  2,  2,
	  -1, -1,  2,  2,  2,  2,  2,
	  -1,  2,  2,  2,  2,  2, -1,
	  2,   2,  2,  2,  2, -1, -1,
	  2,   2,  2,  2, -1, -1, -1,
	  2,   2,  2, -1, -1, -1, -1
	};

	cv::Mat kernel(7,7,CV_32F, kdata);

	// 2D filter
	cv::filter2D(img_in, img_out, CV_32F, kernel);

}

// -----------------------------------
//   90 Degree Thin Line Filter
// -----------------------------------

void thinLine_filter_90Degrees(cv::Mat& img_in, cv::Mat&  img_out) {


	// 2D kernel for 90 degree lines
	float kdata[]= {
	  -1, -1, 2, 2, 2, -1, -1,
	  -1, -1, 2, 2, 2, -1, -1,
	  -1, -1, 2, 2, 2, -1, -1,
	  -1, -1, 2, 2, 2, -1, -1,
	  -1, -1, 2, 2, 2, -1, -1,
	  -1, -1, 2, 2, 2, -1, -1,
	  -1, -1, 2, 2, 2, -1, -1,
	};


	cv::Mat kernel(7,7,CV_32F, kdata);

	// 2D filter
	cv::filter2D(img_in, img_out, CV_32F, kernel);

}

// -----------------------------------
//   90 Degree Thick Line Filter
// -----------------------------------

void thickLine_filter_90Degrees(cv::Mat& img_in, cv::Mat&  img_out) {


	// 2D kernel for 90 degree lines
	float kdata[]= {
	  -1, 2, 2, 2, 2, 2, -1,
	  -1, 2, 2, 2, 2, 2, -1,
	  -1, 2, 2, 2, 2, 2, -1,
	  -1, 2, 2, 2, 2, 2, -1,
	  -1, 2, 2, 2, 2, 2, -1,
	  -1, 2, 2, 2, 2, 2, -1,
	  -1, 2, 2, 2, 2, 2, -1,
	};


	cv::Mat kernel(7,7,CV_32F, kdata);

	// 2D filter
	cv::filter2D(img_in, img_out, CV_32F, kernel);

}


// -----------------------------------
// 	135 Degree Thin Line Filter
// -----------------------------------

void thinLine_filter_135Degrees(cv::Mat& img_in, cv::Mat&  img_out) {

	// 2D kernel for 135 degree lines
	float kdata[]= {
	   2,   2, -1, -1, -1,  -1,  -1,
	   2,   2,  2, -1, -1,  -1,  -1,
	  -1,   2,  2,  2, -1,  -1,  -1,
	  -1,  -1,  2,  2,  2,  -1,  -1,
	  -1,  -1, -1,  2,  2,   2,  -1,
	  -1,  -1, -1, -1,  2,   2,   2,
	  -1,  -1, -1, -1, -1,   2,   2
	};


	cv::Mat kernel(7,7,CV_32F, kdata);

	// 2D filter
	cv::filter2D(img_in, img_out, CV_32F, kernel);

}

// -----------------------------------
// 	135 Degree Thick Line Filter
// -----------------------------------

void thickLine_filter_135Degrees(cv::Mat& img_in, cv::Mat&  img_out) {

	// 2D kernel for 135 degree lines
	float kdata[] = {
	   2,   2,  2, -1, -1,  -1,  -1,
	   2,   2,  2,  2, -1,  -1,  -1,
	   2,   2,  2,  2,  2,  -1,  -1,
	  -1,   2,  2,  2,  2,   2,  -1,
	  -1,  -1,  2,  2,  2,   2,   2,
	  -1,  -1, -1,  2,  2,   2,   2,
	  -1,  -1, -1, -1,  2,   2,   2
	};

	cv::Mat kernel(7,7,CV_32F, kdata);

	// 2D filter
	cv::filter2D(img_in, img_out, CV_32F, kernel);

}
