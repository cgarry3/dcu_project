#ifndef MY_VEHICLE
#define MY_VEHICLE

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class vehicle {
public:

	// ----------------------------------------
    //  Functions
	// ----------------------------------------

    vehicle(std::vector<cv::Point> _contour);
    void predictNextCenterPoint();

	// ----------------------------------------
    //  States
	// ----------------------------------------

    // current contour of the vehicle
	std::vector<cv::Point> currentContour;

    // Center point of the vehicle
    std::vector<cv::Point> centerPositions;

    // Used for creating tracking box
    double dblCurrentDiagonalSize;
    double dblCurrentAspectRatio;
    cv::Rect currentBoundingRect;

    // To indicate vehicle has been found
    bool blnCurrentMatchFoundOrNewVehicle;

    // To indicate it is tracked
    bool blnStillBeingTracked;

    // Number of frames without a match
    int intNumOfConsecutiveFramesWithoutAMatch;

    // Predicted next center point of vehicle
    cv::Point predictedNextCenterPoint;

};

#endif    // MY_VEHICLE
