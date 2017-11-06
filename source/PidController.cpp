#include "Geometry.h"
#include <cmath>

double bezier_calc(double wheelAngle, double laneCenter, double laneAngle, double progressPoint) {
	//deatils: en.wikipedia.org/wiki/B%C3%A9zier_curve
	//Check incoming values which need to be restricted for themath to work.
	if (progressPoint > 1)
		return NULL;
	if (progressPoint < 0)
		return NULL;
	//Should fix to be better responses with details, and to use or statement.
	//Step Zero define variables
	double PointZero_x = 540; //needs to be set for middle of the video, which I'm assuming is about 540 (1080/2) for now.
	double PointZero_y = 0; //Doesn't ever change.
	double PointOne_x = 0;
	double PointOne_y = 0;
	double PointTwo_x = 0;
	double PointTwo_y = 0;
	double PointThree_x = 0;
	PointThree_x = laneCenter;
	double PointThree_y = 120; //might be static, but needs to be set based on where we have confidence the center of the lane is vertically
	double Result_x = 0;
	double Result_y = 0;
	double OutputAngle = 0;
	//Converting angles from degrees to radians ???
	wheelAngle = Geometry::DegToRad(wheelAngle);
	laneAngle = Geometry::DegToRad(laneAngle);
	//Step one, get all 4 points  (the 30's should eventually be replaced by half of PointThree_y (or some other value) but that all depends on if we end up getting that in as a variable)
	PointOne_x = PointZero_x + 30 * sin(wheelAngle);
	PointOne_y = PointZero_y + 30 * cos(wheelAngle);
	PointTwo_x = PointThree_x - 30 * sin(laneAngle);
	PointTwo_y = PointThree_y - 30 * cos(laneAngle);
	//Step two, do the math.  Better now, that I found out the pow(a,b) function works by outputting a^b
	Result_x = pow(1 - progressPoint, 3) * PointZero_x + 3 * pow(1 - progressPoint, 2) * progressPoint * PointOne_x + 3 * (1 - progressPoint) * pow(progressPoint, 2) * PointTwo_x + pow(progressPoint, 3) * PointThree_x;
	Result_y = pow(1 - progressPoint, 3) * PointZero_y + 3 * pow(1 - progressPoint, 2) * progressPoint * PointOne_y + 3 * (1 - progressPoint) * pow(progressPoint, 2) * PointTwo_y + pow(progressPoint, 3) * PointThree_y;
	//Step three, do the monster math (use the resultant location to calculate an angle to which the wheels should steer)
	OutputAngle = (90 - Geometry::RadToDeg(atan(Result_y / (Result_x - PointZero_x))));
	if (OutputAngle >= 90) {
		OutputAngle = -(180 - OutputAngle);
	};
	//Some debugging code from earlier.
	/*
	std::cout << Result_x;
	std::cout << ',';
	std::cout << Result_y;
	std::cout << '\n';
	//I'd like to keep this next bit because I think I'm going to have to change some of how the math works in the future, and being able to have the angle at an arbitrary point along the curve is useful.
	double tangent_angle;
	tangent_angle = ((3 * pow(1 - progressPoint, 2) * (PointOne_y - PointZero_y)) + (6 * (1 - progressPoint)*progressPoint * (PointTwo_y - PointOne_y)) + (3 * pow(progressPoint, 2)*(PointThree_y - PointTwo_y))) / ((3 * pow(1 - progressPoint, 2) * (PointOne_x - PointZero_x)) + (6 * (1 - progressPoint)*progressPoint * (PointTwo_x - PointOne_x)) + (3 * pow(progressPoint, 2)*(PointThree_x - PointTwo_x)));
	std::cout << Geometry::RadToDeg(atan(tangent_angle));
	std::cout << '\n';
	*/
	return OutputAngle;
}

//some sanity checks
//bezier_calc(0, 540, 0, 0.5); //should output with 0
//bezier_calc(0, 600, 90, 0.5); //should output 55 (ish?)
//bezier_calc(0, 480, -90, 0.5); //should output -55 (ish?)
