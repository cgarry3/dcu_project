#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace cv;
using namespace std;

typedef unsigned int uint32;

int main()
{

    cv::Mat inImg =    cv::imread("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic.jpg");

    int width = inImg.rows;
    int height = inImg.cols;

    //Data point copy
    unsigned char * pData = (unsigned char *)inImg.data;
    unsigned char * Dest   = (unsigned char *)0x0f000000;;
    Dest = (unsigned char *)malloc(sizeof(unsigned char)*width*height*3);


    //data copy using memcpy function
    memcpy(Dest, pData, sizeof(unsigned char)*width*height*3);


    std::cout << "copied to 0x0f000000 from input image\n";


    cv::Mat outImg(width, height, CV_8UC3);
    //data copy using memcpy function
    memcpy(outImg.data, Dest, sizeof(unsigned char)*width*height*3);

    std::cout << "copied from 0x0f000000 to output image\n";

    //processing and copy check
    cv::namedWindow("Test");
    imshow("Test", inImg);

    cv::namedWindow("Test2");
    imshow("Test2", outImg);

    cvWaitKey(0);

    std::cout << "finished\n";

}
