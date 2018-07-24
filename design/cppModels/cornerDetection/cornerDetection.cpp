// =======================================================
// Author:       Cathal Garry
// Email:        cathal.garry3@mail.dcu.ie
// Date:         10 Dec 2017
// Description:
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




// --------------------------------------------------------------------
// Function Declarations
// --------------------------------------------------------------------

void addCountToImage(int leftLaneVehicleCount,int rightLaneVehicleCount, cv::Mat &imgFrame);
void cornerDetection(cv::Mat &imgIn, cv::Mat &imgOut, int rows, int cols);



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
    //  Program Execution
    // ---------------------------------------------------------------------

    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        cv::Mat imgFrame1Copy = inpuFrame1.clone();

        // -------------------------------------------------------
        //  Stage 1:  Corner Detection
        // -------------------------------------------------------


        // Corner Detection
        cornerDetection(imgFrame1Copy, imgFrame1Copy, rows, cols);


        // Show Processed Image
        cv::imshow("Corner Detection", imgFrame1Copy);

        // -------------------------------------------------------
        //  Stage 2:  Save Image
        // -------------------------------------------------------

        std::ostringstream os;
        os << "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\cornerDetection.jpg";
        std::string path = os.str();

        cv::imwrite(path, imgFrame1Copy);

        // -------------------------------------------------------
        //  Stage 3:  Update for next frame
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
//  Corner Detection
// --------------------------------------------------------------------------------------------

void cornerDetection(cv::Mat &imgIn, cv::Mat &imgOut, int rows, int cols)
{

	// image variables
        cv::Mat imgGray;

        // threshold values
        int thresh = 75;
        int max_thresh = 255;

	// ------------------------------------------
	// Start Execution time
	// ------------------------------------------

	clock_t tStart = clock();

	// ------------------------------------------
	// RGB to grayscale conversion
	// ------------------------------------------

	cv::cvtColor( imgIn, imgGray, cv::COLOR_BGR2GRAY );

	// ------------------------------------------
	// Corner Detection
	// ------------------------------------------

	cv::Mat dst, dst_norm, dst_norm_scaled;
	dst = cv::Mat::zeros( imgIn.size(), CV_32FC1 );
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.04;
	cv::cornerHarris( imgGray, dst, blockSize, apertureSize, k, cv::BORDER_DEFAULT );
	cv::normalize( dst, dst_norm, 0, max_thresh, cv::NORM_MINMAX, CV_32FC1, cv::Mat() );
	convertScaleAbs( dst_norm, dst_norm_scaled );
	for( int j = 0; j < dst_norm.rows ; j++ )
	 { for( int i = 0; i < dst_norm.cols; i++ )
		  {
			if( (int) dst_norm.at<float>(j,i) > thresh )
			  {
			   cv::circle( imgIn, cv::Point( i, j ), 5,  cv::Scalar(0), 2, 8, 0 );
			  }
		  }
	 }

	// ------------------------------------------
	// Output Image
	// ------------------------------------------

	imgOut = imgIn;

	// ------------------------------------------
	// End Execution time
	// ------------------------------------------

	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
}

