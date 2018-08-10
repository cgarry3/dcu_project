// =======================================================
// Author:       Cathal Garry
// Email:        cathal.garry3@mail.dcu.ie
// Date:         10 Dec 2017
// Description:  This is program takes in a image and 
//               detects all the images in the file using
//               the sobel filter
// ========================================================

#include <opencv2/opencv.hpp>
#include <time.h>

using namespace cv;
using namespace std;

// defines
#define REDUCED_GRAN_BOX_SIZE 5

int main()
{
	// ------------------------------------------
	// Images
	// ------------------------------------------

        Mat imgIn;
        Mat imgGray;
        Mat imgDilate;
        Mat imgEdges;

	// ------------------------------------------
	// Start Execution time
	// ------------------------------------------

	clock_t tStart = clock();

	// ------------------------------------------
	// Read in Input Image
	// ------------------------------------------


	string testImage = "C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\eval\\testImage_1080p.jpg";
        imgIn = imread(testImage, CV_LOAD_IMAGE_COLOR);   // Read the file

	// ------------------------------------------
	// Convert RGB Image to Grayscale
	// ------------------------------------------

        cvtColor( imgIn, imgGray, cv::COLOR_BGR2GRAY );

	// ------------------------------------------
	// Dilate image
	// ------------------------------------------

        Mat reducedGranularity = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(REDUCED_GRAN_BOX_SIZE, REDUCED_GRAN_BOX_SIZE));

	cv::dilate(imgGray, imgDilate, reducedGranularity);

	// ------------------------------------------
	// Edge Detection
	// ------------------------------------------

	// Storage
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;

        // Gradient X
	cv::Sobel( imgDilate, grad_x, CV_8U, 1, 0);
	cv::convertScaleAbs( grad_x, abs_grad_x );

	// Gradient Y
	cv::Sobel( imgDilate, grad_y, CV_8U, 0, 1);
	cv::convertScaleAbs( grad_y, abs_grad_y );
	   
	// Total Gradient (approximate)
	cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, imgEdges );

	// ------------------------------------------
	// End Execution time
	// ------------------------------------------

	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

	// ------------------------------------------
	// Save New Gray Image
	// ------------------------------------------

	fprintf(stdout, "Image edge detection complete\n");

	// ------------------------------------------
	// Display Both Images
	// ------------------------------------------

	imshow( "Display Window RGB", imgIn );
	imshow( "Display Window Output", imgEdges );

	waitKey(0);
}
