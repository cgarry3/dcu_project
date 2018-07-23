// -------------------------------------------------------------------------
//       DCU White Line Detector IP
//
//  Author:       Cathal Garry
//  Description:  Testbench for white line detect
// ------------------------------------------------------------------------

// --------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include "hls_opencv.h"
#include "whiteLineDetect.h"

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

#define ROWS 1280
#define COLs 720

// --------------------------------------------------------------------
// Function Declarations
// --------------------------------------------------------------------

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

        // DUT
        AXI_STREAM stream_in, stream_out;
        cvMat2AXIvideo(imgIn, stream_in);
        whiteLineDetect(stream_in, stream_out);
        AXIvideo2cvMat(stream_out, imgOut);

        cv::imwrite("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\test.jpg", imgOut);

        // -------------------------------------------------------
        //  Step 2: Display Images with information
        // -------------------------------------------------------

        // DUT input
        cv::imshow("Original Image", imgIn);

        // DUT output
        cv::imshow("White Line Image", imgOut);

        // Save image
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

// --------------------------------------------------
//  Convert number to String
// --------------------------------------------------

string NumberToString ( int Number )
 {
    ostringstream ss;
    ss << Number;
    return ss.str();
 }
