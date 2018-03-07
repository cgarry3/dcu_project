
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main()
{
	// Read in input image
	IplImage* im_gray_src = cvLoadImage("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* im_gray_dst = cvCreateImage(cvGetSize(im_gray_src), im_gray_src->depth, im_gray_src->nChannels);;
	size_t sizeInBytes = im_gray_src->imageSize;

	std::cout << "Source Image address: " << im_gray_src << "\n";
	std::cout << "Destination Image address: " << im_gray_dst << "\n";
	std::cout << "Size of Image in bytes: " << sizeInBytes << "\n";

	cvCopy(im_gray_src , im_gray_dst, NULL);
	cvSaveImage("C:\\Users\\cgarry\\Desktop\\copied_image.jpg",im_gray_dst);

}
