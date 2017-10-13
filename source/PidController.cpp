#include "Geometry.h"
#include <cmath>

double bezier_calc(double wheelAngle, double laneCenter, double laneAngle, double progressPoint) {
	//deatils: en.wikipedia.org/wiki/B%C3%A9zier_curve
	//Step Zero define variables
	double PointZero_x = 240; //needs to be set for middle of the video.
	double PointZero_y = 0; //Doesn't ever change.
	double PointOne_x = 0;
	double PointOne_y = 0;
	double PointTwo_x = 0;
	double PointTwo_y = 0;
	double PointThree_x = 0;
	double PointThree_y = 60; //Will be static, but needs to be set based on where we have confidence the center of the lane is vertically
	double Result_x = 0;
	double Result_y = 0;
	double OutputAngle = 0;
	//Step one, get all 4 points  (the 30's should eventually be replaced by half of PointThree_y but that all depends on if we end up getting that in as a variable)
	PointOne_x = PointZero_x + 30 * sin(wheelAngle);
	PointOne_y = PointZero_y + 30 * cos(wheelAngle);
	PointThree_x = laneCenter;
	PointTwo_x = PointThree_x - 30 * sin(laneAngle);
	PointTwo_y = PointThree_y - 30 * cos(laneAngle);
	//Step two, do the math.  Better now, that I found out the pow(a,b) function works by outputting a^b
	Result_x = pow(1 - progressPoint, 3) * PointZero_x + 3 * pow(1 - progressPoint, 2) * progressPoint * PointOne_x + 3 * (1 - progressPoint) * pow(progressPoint, 2) * PointTwo_x + pow(progressPoint, 3) * PointThree_x;
	Result_y = pow(1 - progressPoint, 3) * PointZero_y + 3 * pow(1 - progressPoint, 2) * progressPoint * PointOne_y + 3 * (1 - progressPoint) * pow(progressPoint, 2) * PointTwo_y + pow(progressPoint, 3) * PointThree_y;
	//Step three, do the monster math (use the resultant location to calculate an angle to which the wheels should steer)
	OutputAngle = atan(Result_y / (Result_x - PointZero_x));
	//All done?
	return OutputAngle;
}