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
        int leftCount;
        int rightCount;

        // DUT
        AXI_STREAM stream_in, stream_out;
        cvMat2AXIvideo(imgIn, stream_in);
        vehicleCount(stream_in, stream_out, leftCount, rightCount,result);
        AXIvideo2cvMat(stream_out, imgOut);

        // -------------------------------------------------------
        //  Step 2: Process result from DUT
        // -------------------------------------------------------

        std::cout << "Result is: " << result << std::endl;
        std::cout << "Left count is: " << leftCount << std::endl;
        std::cout << "Right count is: " << rightCount << std::endl;




        // -------------------------------------------------------
        //  Step 3: Display Images with information
        // -------------------------------------------------------

        // Add count to input image
        addCountToImage( leftCount , rightCount, imgIn);
        cv::imshow("Vehicle Counts", imgIn);

        // Show output from DUT
        cv::imshow("DUT output Image", imgOut);

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


string NumberToString ( int Number )
 {
    ostringstream ss;
    ss << Number;
    return ss.str();
 }
