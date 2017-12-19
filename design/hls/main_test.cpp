#include <opencv2/opencv.hpp>
#include "rgb2grayScale.h"
#include "hls_opencv.h"
#include <iostream>
#include <stdio.h>


using namespace cv;
using namespace std;

// AXI stream version
AXI_STREAM src_axi, dst_axi;

int main()
{
	// ------------------------------------------
	//  Images
	// ------------------------------------------

	IplImage *im_rgb     = cvLoadImage("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic.bmp");
    IplImage *im_golden  = cvLoadImage("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic_grayScale.jpg");
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

    cvSaveImage("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic_test.jpg",im_output);
	fprintf(stdout, "Image converted to gray\n");


	// ------------------------------------------
	// Convert to Mat Images
	// ------------------------------------------

	cv::Mat result;
	const cv::Mat img3 = cvarrToMat(im_rgb);
	const cv::Mat img1 = cvarrToMat(im_golden);
	const cv::Mat img2 = cvarrToMat(im_output);

	// ------------------------------------------
	// Display Both Images
	// ------------------------------------------

	namedWindow( "Display Window RGB Image", WINDOW_AUTOSIZE );
	namedWindow( "Display Window Reference Image", WINDOW_AUTOSIZE );
	namedWindow( "Display Window Output Image", WINDOW_AUTOSIZE );
	imshow( "Display Window Reference Image", img1 );
	imshow( "Display Window Output Image", img2 );
	imshow( "Display Window RGB Image", img3 );

	waitKey(0);

	return 0;
}
