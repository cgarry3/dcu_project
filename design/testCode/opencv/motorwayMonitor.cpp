// =======================================================
// Author:       Cathal Garry
// Email:        cathal.garry3@mail.dcu.ie
// Date:         10 Dec 2017
// Description:  This is program counts vehicles on the
//               M6 in England. The program is built using
//               existing openCV function and implements a
//               a count for the left and right lane.
// ========================================================

// --------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include "motorwayTraffic.h"

#include<iostream>
#include <time.h>

// --------------------------------------------------------------------
// Constants
// --------------------------------------------------------------------

const cv::Scalar COLOUR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar COLOUR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar COLOUR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar COLOUR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar COLOUR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar COLOUR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

// --------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------

#define REDUCED_GRAN_BOX_SIZE 5
#define ROWS 1280
#define COLs 720

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
    capVideo.open("/home/xilinx/work/test/eval/testVideo_1080p.mp4");


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

        int result;

        // check for cars in each lane
        checkIfvehiclesCrossedOnLine(imgFrame1Copy, result);

        std::cout << "Result is: " << result << std::endl;

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

        //cv::imshow("Count values", imgFrame1Copy);

        // show regions of interest
        addROIToImage(imgFrame1Copy, rects, rectsSize);

        //cv::imshow("ROI of Interests", imgFrame1Copy);



        // Check for the end of the video
        if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
            capVideo.read(inpuFrame1);
        } else {
            std::cout << "end of video\n";
            break;
        }

        frameCount++;
        chCheckForEscKey = cv::waitKey(1);

    }
    return(0);
}

// --------------------------------------------------------------------------------------------
// Check if vehicle crossed either count lines by just using the image
// --------------------------------------------------------------------------------------------

void checkIfvehiclesCrossedOnLine(cv::Mat &imgFrame, int &result){

	// image variables
    cv::Mat imgGray;
    cv::Mat imgDilate;
    cv::Mat imgEdges;
    cv::Mat imgThreshold;

	// ------------------------------------------
	// Start Execution time
	// ------------------------------------------

	clock_t tStart = clock();

    // -------------------------------------------------------
    //  Stage 1:  Turn Gray
    // -------------------------------------------------------

    cv::cvtColor( imgFrame, imgGray, cv::COLOR_BGR2GRAY );

    // -------------------------------------------------------
    //  Stage 3:  Create image of edge features
    // -------------------------------------------------------


    // Storage
    cv::Mat grad_x, grad_y, grad;
    cv::Mat abs_grad_x, abs_grad_y;

    // Gradient X
    cv::Sobel( imgDilate, grad_x, CV_8U, 1, 0);
    cv::convertScaleAbs( grad_x, abs_grad_x );

    // Gradient Y
    cv::Sobel( imgDilate, grad_y, CV_8U, 0, 1);
    cv::convertScaleAbs( grad_y, abs_grad_y );

    // Total Gradient (approximate)
    cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad )
	
    // show image
    //cv::imshow("Edge Detection", imgEdges);

    // -------------------------------------------------------
    //  Stage 2:  Reduce granularity
    // -------------------------------------------------------

    cv::Mat reducedGranularity = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(REDUCED_GRAN_BOX_SIZE, REDUCED_GRAN_BOX_SIZE));

    // Dilating and erode the images
	cv::dilate(imgEdges, imgDilate, reducedGranularity);
	cv::dilate(imgDilate, imgDilate, reducedGranularity);
	cv::erode(imgDilate, imgDilate, reducedGranularity);

    // show image
    //cv::imshow("Reduced Granularity", imgDilate);

    // -------------------------------------------------------
    //  Stage 3:  Set all highed values to all white
    // -------------------------------------------------------

    cv::threshold(imgDilate, imgThreshold, 30, 254.0, CV_THRESH_BINARY);

    // show image
    //cv::imshow("Threshold Image", imgThreshold);

    // -------------------------------------------------------
    //  Stage 4:  Determine if car has passed a line in an image
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
				cv::Scalar colour = imgThreshold.at<uchar>(y+ROIHEIGHT,x+i);
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
				cv::Scalar colour = imgThreshold.at<uchar>(y+ROIHEIGHT,x+i);
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
	// Adding Left Lane Vehicle Count
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
	// Adding Left Lane Vehicle Count
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
