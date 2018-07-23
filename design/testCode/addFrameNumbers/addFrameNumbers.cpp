// =======================================================
// Author:       Cathal Garry
// Email:        cathal.garry3@mail.dcu.ie
// Date:         10 Dec 2017
// Description:  C++ application for adding frame
//               numbers to image frame in a video
// ========================================================

// --------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include<iostream>


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

#define ROWS 1280
#define COLs 720




// --------------------------------------------------------------------
// Function Declarations
// --------------------------------------------------------------------

void addCountToImage(int frameCount, cv::Mat &imgFrame, int width, int height);


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
    capVideo.open("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\eval\\testVideo_1080p.mp4");


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

    // output video.
    double fps = 60;//capVideo.get(CV_CAP_PROP_FPS);
    int frame_width = capVideo.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = capVideo.get(CV_CAP_PROP_FRAME_HEIGHT);

    cv::VideoWriter videoOut("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\TestVideo_60fps_FrameNumbered.mp4",
    		                  CV_FOURCC('m','p','4','v'),fps, cv::Size(frame_width,frame_height));


    // ---------------------------------------------------------------------
    //  Program Execution
    // ---------------------------------------------------------------------

    while (capVideo.isOpened() && chCheckForEscKey != 27) {
    	// clone input image
        cv::Mat imgFrame1Copy = inpuFrame1.clone();

        // add frame number to clone image
        addCountToImage(frameCount,  imgFrame1Copy, frame_width, frame_height);

        // show image
        cv::imshow("ROI of Interests", imgFrame1Copy);

        // output image
        videoOut << imgFrame1Copy;


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
// Add Frame Count To image
// --------------------------------------------------------------------------------------------

void addCountToImage(int frameCount, cv::Mat &imgFrame, int width, int height) {


	// -------------------------------------------
	// Adding Left Lane Vehicle Count
	// -------------------------------------------

    int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
    double dblFontScale = (imgFrame.rows * imgFrame.cols) / 200000.0;
    int intFontThickness = (int)std::round(dblFontScale * 3);

    cv::Size textSize = cv::getTextSize(std::to_string(frameCount), intFontFace, dblFontScale, intFontThickness, 0);

    cv::Point ptTextBottomLeftPosition;

    ptTextBottomLeftPosition.x = width/2;
    ptTextBottomLeftPosition.y = height-100;

    cv::putText(imgFrame, std::to_string(frameCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, COLOUR_RED, intFontThickness);

}
