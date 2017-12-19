
#include <opencv2/opencv.hpp>
#include <stdio.h>


int main()
{
	// Read in input image
	IplImage* im_gray = cvLoadImage("C:\Users\cgarry\Google Drive\dcu_masters\project\design\image_algorthim\data\M6_traffic.jpg",CV_LOAD_IMAGE_GRAYSCALE);

	// Convert RGB image to grayscale
	IplImage *im_rgb  = cvLoadImage("C:\Users\cgarry\Google Drive\dcu_masters\project\design\image_algorthim\data\M6_traffic.jpg");
	IplImage *im_gray = cvCreateImage(cvGetSize(im_rgb),IPL_DEPTH_8U,1);
	cvCvtColor(im_rgb,im_gray,CV_RGB2GRAY);

	// convert to Binary
	IplImage* im_bw = cvCreateImage(cvGetSize(im_gray),IPL_DEPTH_8U,1);
	cvThreshold(im_gray, im_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	// Save to disk
	cvSaveImage("C:\Users\cgarry\Google Drive\dcu_masters\project\design\image_algorthim\data\image_bw.jpg",im_bw);

	fprintf(stdout, "Image converted to gray\n");
}
