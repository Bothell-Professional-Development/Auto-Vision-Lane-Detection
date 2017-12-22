#include "Geometry.h"
#include <cmath>
#include <iostream>

//defining some stuff I'll need later
bool newLaneCenterAvailable = 0; //set to 1 when we get a new center of lane sent to us from that module.  This can probably be gotten rid of later for a better way of interrupting the loop below.
double outputAngle = 0; //This is the angle which will output to the actual thing which will steer the vehicle.

double bezier_calc(double wheelAngle, double laneCenter, double laneAngle, double progressPoint) {
	//deatils: en.wikipedia.org/wiki/B%C3%A9zier_curve
	//Check incoming values which need to be restricted for the math to work.
	if (progressPoint > 1)
		progressPoint = 1;
	if (progressPoint < 0)
		progressPoint = 0;
	//Anything out of bounds gets snapped to closest in bounds.  This should handle things as nicely as possible.
	//Step Zero define variables used in the fuction.
	double PointZero_x = 540; //needs to be set for middle of the video, which I'm assuming is about 540 (1080/2) for now.
	double PointZero_y = 0; //Doesn't ever change.
	double PointOne_x = 0;
	double PointOne_y = 0;
	double PointTwo_x = 0;
	double PointTwo_y = 0;
	double PointThree_x = 0;
	PointThree_x = laneCenter; //getting a bit ahead of myself, since this is what this point needs to be before the math
	double PointThree_y = 120; //might be static, but needs to be set based on where we have confidence the center of the lane is vertically
	double Result_x = 0;
	double Result_y = 0;
	double OutputAngle = 0;
	//Converting angles from degrees to radians because math.
	wheelAngle = Geometry::DegToRad(wheelAngle);
	laneAngle = Geometry::DegToRad(laneAngle);
	//Step one, get all 4 points  (the 30's should eventually be replaced by some fraction of PointThree_y (or some other value) but that all depends on if we end up getting that in as a variable)
	PointOne_x = PointZero_x + 30 * sin(wheelAngle);
	PointOne_y = PointZero_y + 30 * cos(wheelAngle);
	PointTwo_x = PointThree_x - 30 * sin(laneAngle);
	PointTwo_y = PointThree_y - 30 * cos(laneAngle);
	//Step two, do the math.  Better now, that I found out the pow(a,b) function works by outputting a^b
	//This algorithm now uses the tangent of the smoothest curve path to the center line
	Result_x = (3 * pow(1 - progressPoint, 2) * (PointOne_x - PointZero_x)) + (6 * (1 - progressPoint)*progressPoint * (PointTwo_x - PointOne_x)) + (3 * pow(progressPoint, 2)*(PointThree_x - PointTwo_x));
	Result_y = (3 * pow(1 - progressPoint, 2) * (PointOne_y - PointZero_y)) + (6 * (1 - progressPoint)*progressPoint * (PointTwo_y - PointOne_y)) + (3 * pow(progressPoint, 2)*(PointThree_y - PointTwo_y));
	//Step three, do the monster math (use the resultant location to calculate an angle to which the wheels should steer)
	OutputAngle = (90 - Geometry::RadToDeg(atan(Result_y / (Result_x - PointZero_x))));
	if (OutputAngle >= 90) {
		OutputAngle = -(180 - OutputAngle);
	};
	return OutputAngle;
}

//some sanity checks
//bezier_calc(0, 540, 0, 0.5); //should output with 0
//bezier_calc(0, 600, 90, 0.5); //should output 45 (ish)
//bezier_calc(0, 480, -90, 0.5); //should output -45 (ish)

//some temp initial variable setting.  Eventually we will have to initialize this elsewhere based on real data
//steerAngle = 0;

int PIDController(double steerAngle, double laneCenter, double laneAngle)
{
	int curveProgress = 0; //values from 0-100 for what percentage of the way through the curve the vehicle is (0 is the start, 100 is the end).  Later on, anything over 100 is set to 100, just in case.
	//if we get lots of inputs from the video processing, we can change this to make updates in larger incriments just by changing the two instances of "100" below.
	//Sidebar: does that mean I should have probably used a variable for that instead?
	do {
		steerAngle = bezier_calc(steerAngle, laneCenter, laneAngle, curveProgress / 100); //adjust to the new steering angle
		++curveProgress;
	} while (curveProgress <= 100 && newLaneCenterAvailable == 0);
	steerAngle = bezier_calc(steerAngle, laneCenter, laneAngle, (curveProgress + 100)/2); //Once we get a new update for the lance center, make one last steer command to be at the mid point between where we are on the curve, and the end of the curve.
	//This will probably have to change later to better handle more quickly incoming data.
	return 0;
}