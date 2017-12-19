#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main()
{
	// ------------------------------------------
	// Read in Input Image
	// ------------------------------------------

	IplImage *im_rgb  = cvLoadImage("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic_small.jpg");

	// ------------------------------------------
	// Convert RGB Image to Grayscale
	// ------------------------------------------

    IplImage *im_gray = cvCreateImage(cvGetSize(im_rgb),IPL_DEPTH_8U,1);
	cvCvtColor(im_rgb,im_gray,CV_RGB2GRAY);

	// ------------------------------------------
	// Save New Gray Image
	// ------------------------------------------
	cvSaveImage("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\image_bw.jpg",im_gray);
	fprintf(stdout, "Image converted to gray\n");


	// ------------------------------------------
	// Convert to Mat Images
	// ------------------------------------------

	cv::Mat result;
	const cv::Mat img1 = cvarrToMat(im_rgb);
	const cv::Mat img2 = cvarrToMat(im_gray);

	// ------------------------------------------
	// Display Both Images
	// ------------------------------------------

	namedWindow( "Display Window RGB", WINDOW_AUTOSIZE );
	namedWindow( "Display Window GrayScale", WINDOW_AUTOSIZE );
	imshow( "Display Window RGB", img1 );
	imshow( "Display Window GrayScale", img2 );

	waitKey(0);
}
