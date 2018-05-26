/*
 * motorwayTraffic.h
 *
 *  Created on: 25 May 2018
 *      Author: cgarry
 */

#ifndef MOTORWAYTRAFFIC_H_
#define MOTORWAYTRAFFIC_H_

class motorwayTraffic {
public:
	motorwayTraffic();
	virtual ~motorwayTraffic();
	// methods
	void setNumberOfVehiclesPassed(int);
	int getNumberOfVehiclesPassed();
	void clearNumberOfVehiclesPassed();
	int getNumberOfFramesSinceLineCrossed();
	void setNumberOfFramesSinceLineCrossed(int);
private:
	int numberOfVehiclesPassed;
	int numberOfFramesSinceLineCrossed;
};

#endif /* MOTORWAYTRAFFIC_H_ */
