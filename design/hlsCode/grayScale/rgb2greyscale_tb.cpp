// -------------------------------------------------------------------------
//       DCU Custom RGB to Grayscale Filter
//
//  Author:       Cathal Garry
//  Description:  This is a tesbench for the RGB to grayscale filter
//
// -------------------------------------------------------------------------

#include <opencv2/opencv.hpp>
#include "rgb2grayScale.h"
#include "hls_opencv.h"
#include <iostream>
#include <stdio.h>
#include <ctime>


using namespace cv;
using namespace std;

// ------------------------------------------
//  Defines
// ------------------------------------------

// Testbench mode
//      single image=0
//      video = 1
# define TB_MODE 0

// File paths
# define VIDEO_PATH  "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_Traffic_short.mp4"
# define SINGLE_IMAGE "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic.jpg"
# define GOLDEN_IMAGE "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic_grayscale.jpg"
# define OUTPUT_IMAGE "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic_test.jpg"

// Uncomment to show images when running testbench
//# define SHOW_RGB_IMAGE
//# define SHOW_DUT_IMAGE
//# define SHOW_GOLD_IMAGE

// Max number of frames processed in a video
# define MAX_NUM_FRAMES 70

// ------------------------------------------
// AXI stream version
// ------------------------------------------

AXI_STREAM src_axi, dst_axi;

// ------------------------------------------
// Function for creating golden image
// ------------------------------------------

void rgb2grayscale_test(Mat &inputImage, Mat &outputImage)
{
	Mat gray_refFrame;
    cvtColor(inputImage, gray_refFrame, CV_RGB2GRAY);
    cvtColor(gray_refFrame, outputImage, CV_GRAY2RGB);
}

// ------------------------------------------
// Video Testbench
// ------------------------------------------

#if  TB_MODE==1
	int main()
	{
		// ------------------------------------------
	    // Start Time
		// ------------------------------------------
	    time_t start_sim = time(0);
	    char* dt_start = ctime(&start_sim);

	    cout << "Simulaton Start Time: " << dt_start << endl;

		// ------------------------------------------
		// Loading Video File
		// ------------------------------------------

		VideoCapture capVideo;
		capVideo.open(VIDEO_PATH);


		// checking if the video file is open
		if (!capVideo.isOpened()) {
			std::cout << "error reading video file" << std::endl << std::endl;
			return(0);
		}

		// Checking there is at least one frame
		if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 1) {
			std::cout << "error: video file must have at least two frames";
			return(0);
		}

		// ------------------------------------------
		// Setting up Single Input Image
		// ------------------------------------------

		Mat imgFrame1;
		capVideo.read(imgFrame1);

		// ------------------------------------------
		// Testbench Controls Settings
		//  - chCheckForEscKey is used to end the sim if the esc key is pressed
		//  - frameCount is used to count the frame being processed
		// ------------------------------------------

		char chCheckForEscKey = 0;
		int frameCount        = 1;

		while (capVideo.isOpened() && chCheckForEscKey != 27) {

			cout << "Starting Frame: " << frameCount << endl;

			// ------------------------------------------
			//  Images
			// ------------------------------------------

			// Converting from Mat to IpImage
			IplImage ipltemp     = imgFrame1;
			IplImage *im_rgb     = cvCreateImage(cvSize(imgFrame1.cols,imgFrame1.rows),8,3);
			cvCopy(&ipltemp,im_rgb);

			// Creating output image
			IplImage *im_output  = cvCreateImage(cvGetSize(im_rgb), im_rgb->depth, im_rgb->nChannels);

			// Checking image has data
			if(! im_rgb->imageData)
			{
				cout <<  "Could not open or find the image" << std::endl ;
				return -1;
			}

			// ------------------------------------------
			// Convert the IpImage to AXI4 Stream data
			//-------------------------------------------

			IplImage2AXIvideo(im_rgb, src_axi);

			// ------------------------------------------
			//     DUT
			// ------------------------------------------

			rgb2grayScale_filter(src_axi, dst_axi);


			// ------------------------------------------
			// Convert the AXI4 Stream data to IpImage
			//-------------------------------------------

			AXIvideo2IplImage(dst_axi, im_output);

			// ------------------------------------------
			// Save New Gray Image
			// ------------------------------------------

			cvSaveImage(OUTPUT_IMAGE,im_output);
			fprintf(stdout, "Image converted to gray\n");


			// ------------------------------------------
			// Convert Output to Mat Images
			// ------------------------------------------

			const Mat img_rgb  = cvarrToMat(im_rgb);
			const Mat img_dut  = cvarrToMat(im_output);
			const Mat img_gold = cvarrToMat(im_output);

			// ------------------------------------------
			// Display Images
			// ------------------------------------------

			#ifdef SHOW_RGB_IMAGE
					namedWindow( "Display Window RGB Image", WINDOW_AUTOSIZE );
					imshow( "Display Window RGB Image", img_rgb );
			#endif
			#ifdef SHOW_DUT_IMAGE
					namedWindow( "Display Window DUT Image", WINDOW_AUTOSIZE );
					imshow( "Display Window DUT Image", img_dut );
			#endif
			#ifdef SHOW_GOLD_IMAGE
					namedWindow( "Display Window Golden Image", WINDOW_AUTOSIZE );
					imshow( "Display Window Golden Image", img_gold );
			#endif

			// ------------------------------------------
			// Comparing the test image to the DUT image
			// ------------------------------------------

			Mat diff;
			Mat golden_image;
			rgb2grayscale_test(imgFrame1,golden_image);

			// Diff of the two images
			absdiff(img_dut, golden_image, diff);


			// Splitting diff into four seperate channels
			vector<Mat> channels(4);
			split(diff, channels);

			// Getting the % diff based off channel 0
			float percentage  = (countNonZero(channels[0])/(channels[0].rows*channels[0].cols)) * 100;
			if(percentage<=1)
			{
				cout << "Test Passed for Frame: " << frameCount << endl;
			}
			else
			{
				cout << "Test Failed for Frame: " << frameCount << endl;
				break;
			}
			cout << "Diff of Golden and DUT image: " << percentage << "%" << endl;


			// ------------------------------------------------
			// Settings for the next frame
			// ------------------------------------------------

			if (frameCount < MAX_NUM_FRAMES) {
				capVideo.read(imgFrame1);
			} else {
				std::cout << "end of video\n";
				break;
			}

			frameCount++;
			chCheckForEscKey = waitKey(1);
		}

		if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
			waitKey(0);                         // hold the windows open to allow the "end of video" message to show
		}

		// ------------------------------------------
	    // Start Time
		// ------------------------------------------
	    time_t end_sim = time(0);
	    char* dt_end = ctime(&end_sim);

	    cout << "Simulaton End Time: " << dt_end << endl;

		return 0;
	}
#endif
#if TB_MODE==0
	int main()
	{
		// ------------------------------------------
	    // Start Time
		// ------------------------------------------
	    time_t start_sim = time(0);
	    char* dt_start = ctime(&start_sim);

	    cout << "Simulaton Start Time: " << dt_start << endl;

		// ------------------------------------------
		//  Images
		// ------------------------------------------

		IplImage *im_rgb     = cvLoadImage(SINGLE_IMAGE);
		IplImage *im_golden  = cvLoadImage(GOLDEN_IMAGE);
		IplImage *im_output  = cvCreateImage(cvGetSize(im_rgb), im_rgb->depth, im_rgb->nChannels);

		if(! im_rgb->imageData)                              // Check for invalid input
		{
			cout <<  "Could not open or find the image" << std::endl ;
			return -1;
		}

		IplImage2AXIvideo(im_rgb, src_axi);

		// ------------------------------------------
		//     DUT
		// ------------------------------------------

		rgb2grayScale_filter(src_axi, dst_axi);


		// ------------------------------------------
		// Convert the AXI4 Stream data to OpenCV format
		//-------------------------------------------

		AXIvideo2IplImage(dst_axi, im_output);

		// ------------------------------------------
		// Save New Gray Image
		// ------------------------------------------

		cvSaveImage(OUTPUT_IMAGE,im_output);
		fprintf(stdout, "Image converted to gray\n");


		// ------------------------------------------
		// Convert to Mat Images
		// ------------------------------------------

		cv::Mat result;
		const cv::Mat img_rgb    = cvarrToMat(im_rgb);
		const cv::Mat img_golden = cvarrToMat(im_golden);
		const cv::Mat img_dut    = cvarrToMat(im_output);

		// ------------------------------------------
		// Display Both Images
		// ------------------------------------------

		namedWindow( "Display Window RGB Image", WINDOW_AUTOSIZE );
		namedWindow( "Display Window Golden Image", WINDOW_AUTOSIZE );
		namedWindow( "Display Window DUT Image", WINDOW_AUTOSIZE );
		imshow( "Display Window RGB Image", img_rgb );
		imshow( "Display Window DUT Image", img_dut );
		imshow( "Display Window Golden Image", img_golden  );

		waitKey(0);

		// ------------------------------------------
	    // Start Time
		// ------------------------------------------
	    time_t end_sim = time(0);
	    char* dt_end = ctime(&end_sim);

	    cout << "Simulaton End Time: " << dt_end << endl;

		return 0;
	}
#endif
