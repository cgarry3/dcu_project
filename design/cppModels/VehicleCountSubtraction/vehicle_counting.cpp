// =======================================================
// Author:       Cathal Garry
// Email:        cathal.garry3@mail.dcu.ie
// Date:         10 Dec 2017
// Description:  This is program counts vehicles on the
//               M6 in England. The program is built using
//               existing openCV function and implements a
//               a count for the left and right lane.
// ========================================================

// --------------------------------------------------------------------
// Includes
// --------------------------------------------------------------------

#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/core/core.hpp>
#include "vehicle.h"
#include<iostream>

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

# define REDUCED_GRAN_BOX_SIZE 5


// --------------------------------------------------------------------
// Function Declarations
// --------------------------------------------------------------------

void addNonExistingvehicles(std::vector<vehicle> &existingvehicles, std::vector<vehicle> &currentFramevehicles);
void UpdateExistingvehicles(vehicle &currentFramevehicle, std::vector<vehicle> &existingvehicles, int &intIndex);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
bool checkIfvehiclesCrossedTheLine(std::vector<vehicle> &vehicles, int &countLinePosition, int &numberOfCols, int &leftLaneVehicleCount, int &rightLaneVehicleCount);
void addInfoToImage(std::vector<vehicle> &vehicles,int &leftLaneVehicleCount,int &rightLaneVehicleCount, cv::Mat &imgFrame);


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

	// Vector for holding the vehicle objects
    std::vector<vehicle> vehicles;

    // used to check for esc key
    // if pressed the program closes
    char chCheckForEscKey = 0;

    // indicates if it is the first frame
    bool blnFirstFrame = true;

    // frame count
    int frameCount = 2;

    // Vehicle counts
    int leftLaneVehicleCount = 0;
    int rightLaneVehicleCount = 0;


    // ------------------------------
    //  Video and Image Variables
    // ------------------------------

    // traffic video
    cv::VideoCapture capVideo;

    // Image variables
    cv::Mat imgFrame1;
    cv::Mat imgFrame2;


    // Opening Video file
    capVideo.open("C:\\Users\\cgarry\\Google Drive\\dcu_masters\\project\\design\\image_algorthim\\data\\M6 Motorway Traffic.mp4");

    // Error message if the video file wasn't opened correctly
    if (!capVideo.isOpened()) {
        std::cout << "error reading video file" << std::endl << std::endl;
        return(0);
    }

    // Error Message if there is less than two frames in the video
    if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
        std::cout << "error: video file must have at least two frames";
        //_getch();                   // it may be necessary to change or remove this line if not using Windows
        return(0);
    }

    // Loading the two images
    capVideo.read(imgFrame1);
    capVideo.read(imgFrame2);



    // ------------------------------
    //  Vehicle Count Lines
    // ------------------------------

    // Left and right count lines
    cv::Point leftLaneCountLine[2];
    cv::Point rightLaneCountLine[2];

    // States specific to the video
    int numberOfCols      = imgFrame1.cols;
    int countLinePosition = (int)std::round((double)imgFrame1.rows * 0.55);
    int leftLaneStartPositionOffsetX = 200;
    int leftLaneEndPositionOffsetX = 50;
    int rightLaneStartPositionOffsetX = 50;
    int rightLaneEndPositionOffsetX = 200;


    // Left Lane Count Lines
    // X+Y start location
    leftLaneCountLine[0].x = leftLaneStartPositionOffsetX;
    leftLaneCountLine[0].y = countLinePosition;
    // X+Y End Location
    leftLaneCountLine[1].x = imgFrame1.cols/2-leftLaneEndPositionOffsetX;
    leftLaneCountLine[1].y = countLinePosition;

    // Left Lane Count Lines
    // X+Y start location
    rightLaneCountLine[0].x = imgFrame1.cols/2+rightLaneStartPositionOffsetX;
    rightLaneCountLine[0].y = countLinePosition;
    // X+Y End Location
    rightLaneCountLine[1].x = imgFrame1.cols-rightLaneEndPositionOffsetX;
    rightLaneCountLine[1].y = countLinePosition;




    // ---------------------------------------------------------------------
    //  Program Execution
    // ---------------------------------------------------------------------

    while (capVideo.isOpened() && chCheckForEscKey != 27) {

        std::vector<vehicle> currentFramevehicles;

        cv::Mat imgFrame1Copy = imgFrame1.clone();
        cv::Mat imgFrame2Copy = imgFrame2.clone();

        cv::Mat imgDifference;
        cv::Mat imgThresh;

        // -------------------------------------------------------
        //  Stage 1:  Subtraction
        // -------------------------------------------------------

        // Convert to grayscale
        cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
        cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

        // Blurring the edges
        cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
        cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

        // Diff of the two images
        cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

        // Removing any low value pixels that weren't removed
        cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

        // display subtracted image
        cv::imshow("Subtracted Image", imgThresh);


        // -------------------------------------------------------
        //  Stage 2:  Create List of Vehicles
        // -------------------------------------------------------

        // -----------------------------------
        // Reducing the granularity of the remaining objects
        // -----------------------------------

        cv::Mat reducedGranularity = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(REDUCED_GRAN_BOX_SIZE, REDUCED_GRAN_BOX_SIZE));

        // Dilating and erode the images
		cv::dilate(imgThresh, imgThresh, reducedGranularity);
		cv::dilate(imgThresh, imgThresh, reducedGranularity);
		cv::erode(imgThresh, imgThresh, reducedGranularity);


        // -----------------------------------
        // Getting the outline of each Vehicle
        // -----------------------------------

		// Stage 2 Image(output of image of stage 1)
        cv::Mat imgStage2 = imgThresh.clone();

        // Vector for storing the contours of the vehicles
        std::vector<std::vector<cv::Point> > contours;

        // finds the contours of a the different objects in the image
        cv::findContours(imgStage2, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Vector for storing the convex for the vehicles
        std::vector<std::vector<cv::Point> > convexHulls(contours.size());

        // Removing the fine details of the contours
        for (unsigned int i = 0; i < contours.size(); i++) {
            cv::convexHull(contours[i], convexHulls[i]);
        }

        // -----------------------------------
        // add new vehicles to current vehicle vector
        // -----------------------------------

        for (auto &convexHull : convexHulls) {
            vehicle possiblevehicle(convexHull);

            if (possiblevehicle.currentBoundingRect.area() > 400 &&
                possiblevehicle.dblCurrentAspectRatio > 0.2 &&
                possiblevehicle.dblCurrentAspectRatio < 4.0 &&
                possiblevehicle.currentBoundingRect.width > 30 &&
                possiblevehicle.currentBoundingRect.height > 30 &&
                possiblevehicle.dblCurrentDiagonalSize > 60.0 &&
                (cv::contourArea(possiblevehicle.currentContour) / (double)possiblevehicle.currentBoundingRect.area()) > 0.50) {
                currentFramevehicles.push_back(possiblevehicle);
            }
        }


        // -----------------------------------
        // updating new and old vehicles
        // -----------------------------------

        if (blnFirstFrame == true) {
            for (auto &currentFramevehicle : currentFramevehicles) {
                vehicles.push_back(currentFramevehicle);
            }
        } else {
        	addNonExistingvehicles(vehicles, currentFramevehicles);
        }


        // -------------------------------------------------------
        //  Stage 3:  Count Vehicles in left and right lane
        // -------------------------------------------------------

        // Loading back in the orginal contents
        imgFrame2Copy = imgFrame2.clone();


        // Checking if one vehicle crossed the line
        bool blnAtLeastOnevehicleCrossedTheLine = checkIfvehiclesCrossedTheLine(vehicles, countLinePosition, numberOfCols, leftLaneVehicleCount, rightLaneVehicleCount);

        // Change the colour of the line if a vehicle crosses it
        if (blnAtLeastOnevehicleCrossedTheLine == true) {
            cv::line(imgFrame2Copy, leftLaneCountLine[0], leftLaneCountLine[1], COLOUR_GREEN, 2);
            cv::line(imgFrame2Copy, rightLaneCountLine[0], rightLaneCountLine[1], COLOUR_GREEN, 2);
        } else {
            cv::line(imgFrame2Copy, leftLaneCountLine[0], leftLaneCountLine[1], COLOUR_RED, 2);
            cv::line(imgFrame2Copy, rightLaneCountLine[0], rightLaneCountLine[1], COLOUR_BLUE, 2);
        }

        // Draw input image with details
        addInfoToImage(vehicles, leftLaneVehicleCount, rightLaneVehicleCount, imgFrame2Copy);
        cv::imshow("imgFrame2Copy", imgFrame2Copy);


        // -------------------------------------------------------
        //  Stage 3:  Update for next iteration
        // -------------------------------------------------------

        // Clear the current list of vehicles
        currentFramevehicles.clear();

        // Switch image 2 to image 1
        imgFrame1 = imgFrame2.clone();

        // Check for the end of the video
        if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
            capVideo.read(imgFrame2);
        } else {
            std::cout << "end of video\n";
            break;
        }

        blnFirstFrame = false;
        frameCount++;
        chCheckForEscKey = cv::waitKey(1);

    }
    return(0);
}

// --------------------------------------------------------------------------------------------
// Remove existing vehicles
// --------------------------------------------------------------------------------------------

void addNonExistingvehicles(std::vector<vehicle> &existingvehicles, std::vector<vehicle> &currentFramevehicles) {

	// created predicted position of existing vehicles
    for (auto &existingvehicle : existingvehicles) {
        existingvehicle.blnCurrentMatchFoundOrNewVehicle = false;
        existingvehicle.predictNextCenterPoint();
    }


    for (auto &currentFramevehicle : currentFramevehicles) {

        int intIndexOfLeastDistance = 0;
        double dblLeastDistance = 100000.0;

        // working out distance from current and existing vehicles
        // and assigning an ID based
        for (unsigned int i = 0; i < existingvehicles.size(); i++) {
            if (existingvehicles[i].blnStillBeingTracked == true) {

                double dblDistance = distanceBetweenPoints(currentFramevehicle.centerPositions.back(), existingvehicles[i].predictedNextCenterPoint);

                if (dblDistance < dblLeastDistance) {
                    dblLeastDistance = dblDistance;
                    intIndexOfLeastDistance = i;
                }
            }
        }

        if (dblLeastDistance < currentFramevehicle.dblCurrentDiagonalSize * 0.5) {
        	// Update existing vehicle
        	UpdateExistingvehicles(currentFramevehicle, existingvehicles, intIndexOfLeastDistance);
        }
        else {
        	// add new vehicle
            currentFramevehicle.blnCurrentMatchFoundOrNewVehicle = true;
            existingvehicles.push_back(currentFramevehicle);
        }

    }

    // Updating existing vehicles that didn't have a match
    for (auto &existingvehicle : existingvehicles) {
    	// incrementing the missed match counter
        if (existingvehicle.blnCurrentMatchFoundOrNewVehicle == false) {
            existingvehicle.intNumOfConsecutiveFramesWithoutAMatch++;
        }
        // setting to false if it has been 5 frames without a match
        if (existingvehicle.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
            existingvehicle.blnStillBeingTracked = false;
        }

    }

}

// --------------------------------------------------------------------------------------------
// Add vehicle to existing vehicle list
// --------------------------------------------------------------------------------------------

void UpdateExistingvehicles(vehicle &currentFramevehicle, std::vector<vehicle> &existingvehicles, int &intIndex) {

	// add contour of new vehicle
    existingvehicles[intIndex].currentContour = currentFramevehicle.currentContour;
    existingvehicles[intIndex].currentBoundingRect = currentFramevehicle.currentBoundingRect;

    // add new center position
    existingvehicles[intIndex].centerPositions.push_back(currentFramevehicle.centerPositions.back());

    // Set tracking box
    existingvehicles[intIndex].dblCurrentDiagonalSize = currentFramevehicle.dblCurrentDiagonalSize;
    existingvehicles[intIndex].dblCurrentAspectRatio = currentFramevehicle.dblCurrentAspectRatio;

    // set tracking information
    existingvehicles[intIndex].blnStillBeingTracked = true;
    existingvehicles[intIndex].blnCurrentMatchFoundOrNewVehicle = true;
}


// --------------------------------------------------------------------------------------------
// Distance between two points
// --------------------------------------------------------------------------------------------

double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(pow(intX, 2) + pow(intY, 2)));
}



// --------------------------------------------------------------------------------------------
// Check if vehicle crossed either count lines
// --------------------------------------------------------------------------------------------

bool checkIfvehiclesCrossedTheLine(std::vector<vehicle> &vehicles, int &countLinePosition, int &numberOfCols, int &leftLaneVehicleCount, int &rightLaneVehicleCount) {
    bool blnAtLeastOnevehicleCrossedTheLine = false;

    for (auto vehicle : vehicles) {

        if (vehicle.blnStillBeingTracked == true && vehicle.centerPositions.size() >= 2) {
            int prevFrameIndex = (int)vehicle.centerPositions.size() - 2;
            int currFrameIndex = (int)vehicle.centerPositions.size() - 1;

            // Checks if the vehicle pass the left lane count line
            if (vehicle.centerPositions[prevFrameIndex].y > countLinePosition && vehicle.centerPositions[currFrameIndex].y <= countLinePosition) {
            	if(vehicle.centerPositions[currFrameIndex].x < numberOfCols/2)
            	{
            				leftLaneVehicleCount++;
                            blnAtLeastOnevehicleCrossedTheLine = true;
            	}
            }

            // Checks if the vehicle pass the right lane count line
            if (vehicle.centerPositions[prevFrameIndex].y < countLinePosition && vehicle.centerPositions[currFrameIndex].y >= countLinePosition) {
            	if(vehicle.centerPositions[currFrameIndex].x > numberOfCols/2)
            	{
            				rightLaneVehicleCount++;
                            blnAtLeastOnevehicleCrossedTheLine = true;
            	}

            }
        }

    }

    return blnAtLeastOnevehicleCrossedTheLine;
}


// --------------------------------------------------------------------------------------------
// Add Info To image
// --------------------------------------------------------------------------------------------

void addInfoToImage(std::vector<vehicle> &vehicles,int &leftLaneVehicleCount,int &rightLaneVehicleCount, cv::Mat &imgFrame) {

	// -------------------------------------------
	// Adding regular box to Vehicles on image
	// -------------------------------------------

    for (unsigned int i = 0; i < vehicles.size(); i++) {

        if (vehicles[i].blnStillBeingTracked == true) {
        	// add tracking box
            cv::rectangle(imgFrame, vehicles[i].currentBoundingRect, COLOUR_RED, 2);

            // add tracking number
            int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
            double dblFontScale = vehicles[i].dblCurrentDiagonalSize / 60.0;
            int intFontThickness = (int)std::round(dblFontScale * 1.0);

            cv::putText(imgFrame, std::to_string(i), vehicles[i].centerPositions.back(), intFontFace, dblFontScale, COLOUR_GREEN, intFontThickness);
        }
    }

	// -------------------------------------------
	// Adding Left Lane Vehicle Count
	// -------------------------------------------

    int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
    double dblFontScale = (imgFrame.rows * imgFrame.cols) / 300000.0;
    int intFontThickness = (int)std::round(dblFontScale * 1.5);

    cv::Size textSize = cv::getTextSize(std::to_string(leftLaneVehicleCount), intFontFace, dblFontScale, intFontThickness, 0);

    cv::Point ptTextBottomLeftPosition;

    ptTextBottomLeftPosition.x = 50;//(int)((double)textSize.width * 1.25);
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

    cv::putText(imgFrame, std::to_string(leftLaneVehicleCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, COLOUR_GREEN, intFontThickness);

	// -------------------------------------------
	// Adding Left Lane Vehicle Count
	// -------------------------------------------

    textSize = cv::getTextSize(std::to_string(leftLaneVehicleCount), intFontFace, dblFontScale, intFontThickness, 0);

    ptTextBottomLeftPosition.x = imgFrame.cols - (int)((double)textSize.width * 1.25);
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

    cv::putText(imgFrame, std::to_string(rightLaneVehicleCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, COLOUR_GREEN, intFontThickness);



}

