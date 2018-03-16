#include <opencv2\opencv.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>
#include <opencv2\opencv.hpp>

int main()
{
    cv::Mat inImg =    cv::imread("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6_traffic.jpg");
    //Data point copy
    unsigned char * pData = inImg.data;

    int width = inImg.rows;
    int height = inImg.cols;

    cv::Mat outImg(width, height, CV_8UC3);
    //data copy using memcpy function
    memcpy(outImg.data, pData, sizeof(unsigned char)*width*height*3);

   //processing and copy check
   cv::namedWindow("Test");
   imshow("Test", inImg);

   cv::namedWindow("Test2");
   imshow("Test2", outImg);

   cvWaitKey(0);
}
