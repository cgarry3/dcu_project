// -------------------------------------------------------------------------
//       DCU Vehicle counter IP
//
//  Author:       Cathal Garry
//  Description:  Testbench for vehicle counting IP
// ------------------------------------------------------------------------

// --------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include "hls_opencv.h"
#include "motorwayTraffic.h"
#include "vehicleCount.h"

#include<iostream>

using namespace std;


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

// --------------------------------------------------------------------
// Function Declarations
// --------------------------------------------------------------------

void addCountToImage(int leftLaneVehicleCount,int rightLaneVehicleCount, cv::Mat &imgFrame);
void addROIToImage(cv::Mat &imgFrame, cv::Rect rects[], int rectsSize);
string NumberToString ( int Number );

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
    motorwayTraffic M6RightLane1;
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
    int countLinePosition = 396;//(int)std::round((double)inpuFrame1.rows * 0.55);
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
    cv::Point ROIL0P0 = cv::Point(leftLane0,countLinePosition);
    cv::Point ROIL0P1 = cv::Point(leftLane0+width,countLinePosition);
    cv::Point ROIL0P2 = cv::Point(leftLane0,countLinePosition+height);
    cv::Point ROIL0P3 = cv::Point(leftLane0+width,countLinePosition+height);
    std::vector<cv::Point> points0;
    points0.push_back(ROIL0P0);
    points0.push_back(ROIL0P1);
    points0.push_back(ROIL0P2);
    points0.push_back(ROIL0P3);
    cv::Rect leftLane0Rect =  cv::boundingRect(points0);

    // left lane 1
    cv::Point ROIL1P0 = cv::Point(leftLane1,countLinePosition);
    cv::Point ROIL1P1 = cv::Point(leftLane1+width,countLinePosition);
    cv::Point ROIL1P2 = cv::Point(leftLane1,countLinePosition+height);
    cv::Point ROIL1P3 = cv::Point(leftLane1+width,countLinePosition+height);
    std::vector<cv::Point> points1;
    points1.push_back(ROIL1P0);
    points1.push_back(ROIL1P1);
    points1.push_back(ROIL1P2);
    points1.push_back(ROIL1P3);
    cv::Rect leftLane1Rect =  cv::boundingRect(points1);

    // left lane 2
    cv::Point ROIL2P0 = cv::Point(leftLane2,countLinePosition);
    cv::Point ROIL2P1 = cv::Point(leftLane2+width,countLinePosition);
    cv::Point ROIL2P2 = cv::Point(leftLane2,countLinePosition+height);
    cv::Point ROIL2P3 = cv::Point(leftLane2+width,countLinePosition+height);
    std::vector<cv::Point> points2;
    points2.push_back(ROIL2P0);
    points2.push_back(ROIL2P1);
    points2.push_back(ROIL2P2);
    points2.push_back(ROIL2P3);
    cv::Rect leftLane2Rect =  cv::boundingRect(points2);

    // right lane 0
    cv::Point ROIL3P0 = cv::Point(rightLane0,countLinePosition);
    cv::Point ROIL3P1 = cv::Point(rightLane0+width,countLinePosition);
    cv::Point ROIL3P2 = cv::Point(rightLane0,countLinePosition+height);
    cv::Point ROIL3P3 = cv::Point(rightLane0+width,countLinePosition+height);
    std::vector<cv::Point> points3;
    points3.push_back(ROIL3P0);
    points3.push_back(ROIL3P1);
    points3.push_back(ROIL3P2);
    points3.push_back(ROIL3P3);
    cv::Rect rightLane0Rect =  cv::boundingRect(points3);

    // right lane 1
    cv::Point ROIL4P0 = cv::Point(rightLane1,countLinePosition);
    cv::Point ROIL4P1 = cv::Point(rightLane1+width,countLinePosition);
    cv::Point ROIL4P2 = cv::Point(rightLane1,countLinePosition+height);
    cv::Point ROIL4P3 = cv::Point(rightLane1+width,countLinePosition+height);
    std::vector<cv::Point> points4;
    points4.push_back(ROIL4P0);
    points4.push_back(ROIL4P1);
    points4.push_back(ROIL4P2);
    points4.push_back(ROIL4P3);
    cv::Rect rightLane1Rect =  cv::boundingRect(points4);

    // right lane 2
    cv::Point ROIL5P0 = cv::Point(rightLane2,countLinePosition);
    cv::Point ROIL5P1 = cv::Point(rightLane2+width,countLinePosition);
    cv::Point ROIL5P2 = cv::Point(rightLane2,countLinePosition+height);
    cv::Point ROIL5P3 = cv::Point(rightLane2+width,countLinePosition+height);
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
    //  Testbench Execution
    // ---------------------------------------------------------------------

    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        cv::Mat imgIn = inpuFrame1.clone();
        cv::Mat imgOut = inpuFrame1.clone();

        // -------------------------------------------------------
        //  Step 1: DUT
        // -------------------------------------------------------

        int result;
        int debug=0;

        // DUT
        AXI_STREAM stream_in, stream_out;
        cvMat2AXIvideo(imgIn, stream_in);
        vehicleCount(stream_in, stream_out, debug,result);
        AXIvideo2cvMat(stream_out, imgOut);

        cv::imwrite("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\test.jpg", imgOut);

        // -------------------------------------------------------
        //  Step 2: Process result from DUT
        // -------------------------------------------------------

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

        if((result&0x2)==2)
        {
        	// if its been 5 frames since the line was crossed increase count
        	// otherwise assume it is the same car
			if(M6LeftLane1.getNumberOfFramesSinceLineCrossed()>10)
			{
				M6LeftLane1.setNumberOfVehiclesPassed(M6LeftLane1.getNumberOfVehiclesPassed()+1);
	        	// frames back to 0
				M6LeftLane1.setNumberOfFramesSinceLineCrossed(0);
			}
        }
        else
        {
        	// if no car is seen increase the number of frames since
        	// the line was crossed
        	M6LeftLane1.setNumberOfFramesSinceLineCrossed(M6LeftLane1.getNumberOfFramesSinceLineCrossed()+1);
        }

        if((result&0x4)==4)
        {
        	// if its been 5 frames since the line was crossed increase count
        	// otherwise assume it is the same car
			if(M6LeftLane2.getNumberOfFramesSinceLineCrossed()>10)
			{
				M6LeftLane2.setNumberOfVehiclesPassed(M6LeftLane2.getNumberOfVehiclesPassed()+1);
	        	// frames back to 0
				M6LeftLane2.setNumberOfFramesSinceLineCrossed(0);
			}
        }
        else
        {
        	// if no car is seen increase the number of frames since
        	// the line was crossed
        	M6LeftLane2.setNumberOfFramesSinceLineCrossed(M6LeftLane2.getNumberOfFramesSinceLineCrossed()+1);
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

        if((result&0x16)==16)
        {
        	// if its been 5 frames since the line was crossed increase count
        	// otherwise assume it is the same car
			if(M6RightLane1.getNumberOfFramesSinceLineCrossed()>10)
			{
				M6RightLane1.setNumberOfVehiclesPassed(M6RightLane1.getNumberOfVehiclesPassed()+1);
	        	// frames back to 0
				M6RightLane1.setNumberOfFramesSinceLineCrossed(0);
			}
        }
        else
        {
        	// if no car is seen increase the number of frames since
        	// the line was crossed
        	M6RightLane1.setNumberOfFramesSinceLineCrossed(M6RightLane1.getNumberOfFramesSinceLineCrossed()+1);
        }

        if((result&0x32)==32)
        {
        	// if its been 5 frames since the line was crossed increase count
        	// otherwise assume it is the same car
			if(M6RightLane2.getNumberOfFramesSinceLineCrossed()>10)
			{
				M6RightLane2.setNumberOfVehiclesPassed(M6RightLane2.getNumberOfVehiclesPassed()+1);
	        	// frames back to 0
				M6RightLane2.setNumberOfFramesSinceLineCrossed(0);
			}
        }
        else
        {
        	// if no car is seen increase the number of frames since
        	// the line was crossed
        	M6RightLane2.setNumberOfFramesSinceLineCrossed(M6RightLane2.getNumberOfFramesSinceLineCrossed()+1);
        }

        // -------------------------------------------------------
        //  Step 3: Display Images with information
        // -------------------------------------------------------

        // show regions of interest and cout
        // add count to input image
        int leftCount = M6LeftLane0.getNumberOfVehiclesPassed() + M6LeftLane1.getNumberOfVehiclesPassed() + M6LeftLane2.getNumberOfVehiclesPassed();
        int rightCount = M6RightLane0.getNumberOfVehiclesPassed() + M6RightLane1.getNumberOfVehiclesPassed() + M6RightLane2.getNumberOfVehiclesPassed();
        addCountToImage( leftCount , rightCount, imgIn);

        // add ROI to input image
        addROIToImage(imgIn, rects, rectsSize);

        cv::imshow("ROI of Interests", imgIn);

        // show output from DUT
        cv::imshow("DUT output Image", imgOut);

        cv::imwrite("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\test.jpg", imgOut);

        // -------------------------------------------------------
        //  Step 4: Update for next iteration
        // -------------------------------------------------------

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
// Add Count To image
// --------------------------------------------------------------------------------------------

void addCountToImage(int leftLaneVehicleCount,int rightLaneVehicleCount, cv::Mat &imgFrame) {


	// -------------------------------------------
	// Adding Left Lane Vehicle Count
	// -------------------------------------------

    int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
    double dblFontScale = (imgFrame.rows * imgFrame.cols) / 300000.0;
    int intFontThickness = (int)round(dblFontScale * 1.5);

    cv::Size textSize = cv::getTextSize(NumberToString(leftLaneVehicleCount), intFontFace, dblFontScale, intFontThickness, 0);

    cv::Point ptTextBottomLeftPosition;

    ptTextBottomLeftPosition.x = 50;//(int)((double)textSize.width * 1.25);
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

    cv::putText(imgFrame, NumberToString(leftLaneVehicleCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, COLOUR_GREEN, intFontThickness);

	// -------------------------------------------
	// Adding Left Lane Vehicle Count
	// -------------------------------------------

    textSize = cv::getTextSize(NumberToString(leftLaneVehicleCount), intFontFace, dblFontScale, intFontThickness, 0);

    ptTextBottomLeftPosition.x = imgFrame.cols - (int)((double)textSize.width * 1.25);
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

    cv::putText(imgFrame, NumberToString(rightLaneVehicleCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, COLOUR_GREEN, intFontThickness);



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
            cv::Size textSize = cv::getTextSize(NumberToString(i), fontFace,
                                        fontScale, thickness, &baseline);
            cv::Point textOrg((rects[i].width - textSize.width)/2,
                             (rects[i].height + textSize.height)/2);

            // add text
            cv::putText(imgFrame, NumberToString(i), textOrg, fontFace, fontScale, cv::Scalar::all(255), thickness, 8);
    }


}

string NumberToString ( int Number )
 {
    ostringstream ss;
    ss << Number;
    return ss.str();
 }
