// -------------------------------------------------------------------------
//       DCU MotorWay Class
//
//  Author:       Cathal Garry
//  Description:  Class for keeping track of vehicles on
//                motorway
// ------------------------------------------------------------------------

#include "motorwayTraffic.h"

// -------------------------------------
//   Constructor
// -------------------------------------

motorwayTraffic::motorwayTraffic() {
	// set number of vehicles passed to zero
	numberOfVehiclesPassed=0;

	// set the number of frames since a car passed to 100
	numberOfFramesSinceLineCrossed = 100;
}

// -------------------------------------
//   Destructor
// -------------------------------------

motorwayTraffic::~motorwayTraffic() {
	// does nothing
}

// -------------------------------------
//   get the number of vehicles count
// -------------------------------------

void motorwayTraffic::setNumberOfVehiclesPassed(int inputNumberOfVehiclesPassed)
{
	numberOfVehiclesPassed = inputNumberOfVehiclesPassed;
}

// -------------------------------------
//   Set the number of vehicles count
// -------------------------------------

int motorwayTraffic::getNumberOfVehiclesPassed()
{
	return numberOfVehiclesPassed;
}

// -------------------------------------
//   Used to clear the count
// -------------------------------------

void motorwayTraffic::clearNumberOfVehiclesPassed()
{
	numberOfVehiclesPassed = 0;
}

// -------------------------------------
//   Set the number of frames since the line was passed
// -------------------------------------

int motorwayTraffic::getNumberOfFramesSinceLineCrossed()
{
	return numberOfFramesSinceLineCrossed;
}

// -------------------------------------
//   Get the number of frames since the line was passed
// -------------------------------------

void motorwayTraffic::setNumberOfFramesSinceLineCrossed(int inputNumberOfFramesSinceLineCrossed)
{
	numberOfFramesSinceLineCrossed = inputNumberOfFramesSinceLineCrossed;
}

