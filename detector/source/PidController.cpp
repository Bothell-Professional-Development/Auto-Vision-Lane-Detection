#include "Geometry.h"
#include "PidController.h"
#include <cmath>
#include <iostream>
#include <thread>

double bezier_calc(float progressPoint, float wheelAngle, cv::Point2f &PointZero, cv::Point2f &PointOne, cv::Point2f &PointTwo, cv::Point2f &PointThree)
{
	//deatils: en.wikipedia.org/wiki/B%C3%A9zier_curve
	//Check incoming values which need to be restricted for the math to work.
	if (progressPoint > 1.)
	{
		progressPoint = 1.;
	}
	
	if (progressPoint < 0.)
	{
		progressPoint = 0.;
	}
	//Anything out of bounds gets snapped to closest in bounds.  This should handle things as nicely as possible.
	cv::Point2f Result = { 0, 0 };
	//Converting angles from degrees to radians because math.
	wheelAngle = Geometry::DegToRad(wheelAngle);
	//We're given 3 of the 4 points we need to do our calculations, so this last portion here is what we need to do to speculate on the final point.
	PointOne = { (100.f * sin(wheelAngle) + PointZero.x), (100.f * cos(wheelAngle) + PointZero.y) };
	//Step two, do the math.  Better now, that I found out the pow(a,b) function works by outputting a^b
	//This algorithm now uses the tangent of the smoothest curve path to the center line at the given progress point through the curve.
	Result = (3. * pow(1. - progressPoint, 2.) * (PointOne - PointZero))
		   + (6. * (1. - progressPoint)*progressPoint * (PointTwo - PointOne))
		   + (3. * pow(progressPoint, 2.)*(PointThree - PointTwo));
	//Step three, do the monster math (use the resultant location to calculate an angle to which the wheels should steer)
	double OutputAngle = (90. - Geometry::RadToDeg(atan(Result.y / (Result.x - PointZero.x + 1))));
	//The following might be needed to properly conver the intended steering angle, but that all depends on how the coordinate system works, and what angle we want to pick as "wheels pointing forward"
	/*if (OutputAngle >= 90.)
	{
		OutputAngle = -(180. - OutputAngle);
	};
	*/
	return OutputAngle; 
}

double set_steering_module(double wheelAngle)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1ms);
	return wheelAngle;
}
