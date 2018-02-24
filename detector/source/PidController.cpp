#include "Geometry.h"
#include "PidController.h"
#include <cmath>
#include <iostream>
#include <thread>
#include <limits>
//stuff to display some debug info
#include <opencv2/imgproc.hpp>
#include "proj.h"

double bezier_calc(float wheelAngle, cv::Point2f &PointZero, cv::Point2f &PointOne, cv::Point2f &PointTwo, cv::Point2f &PointThree, double OutputArray[steerCommandArraySize])
{
	//deatils: en.wikipedia.org/wiki/B%C3%A9zier_curve
	cv::Point2f Result = { 0, 0 };
	//We're given 3 of the 4 points we need to do our calculations, so this last portion here is what we need to do to speculate on the final point.
	PointOne = { (BezierMagnitude * sin(wheelAngle) + PointZero.x), (BezierMagnitude * cos(wheelAngle) + PointZero.y) };
	//Properly scaling the magnitute of the point two, so it doesn't completely depend on the box height
	PointTwo = { BezierMagnitude * sin(atan((PointTwo.x - PointThree.x) / (PointTwo.y - PointThree.y + std::numeric_limits<float>::min()))) , BezierMagnitude * cos(atan((PointTwo.x - PointThree.x) / (PointTwo.y - PointThree.y + std::numeric_limits<float>::min()))) };
	//setup the array to hold our results
	for (double n = 0; n < steerCommandArraySize; n++) //important!  n NEEDS to be a double for the math to behave properly.
	{
		//Step two, do the math.  Better now, that I found out the pow(a,b) function works by outputting a^b
		//This algorithm now uses the tangent of the smoothest curve path to the center line at the given progress point through the curve.
		Result = (3. * pow(1. - (n/ steerCommandArraySize), 2.) * (PointOne - PointZero))
			+ (6. * (1. - (n / steerCommandArraySize))*(n / steerCommandArraySize) * (PointTwo - PointOne))
			+ (3. * pow((n / steerCommandArraySize), 2.)*(PointThree - PointTwo));
		//Step three, do the monster math (use the resultant location to calculate an angle to which the wheels should steer)
		double OutputAngle = atan(Result.y / Result.x);
		//The following might be needed to properly convert to the intended steering angle, but that all depends on how the coordinate system works, and what angle we want to pick as "wheels pointing forward"
		/*if (OutputAngle >= 90.)
		{
			OutputAngle = -(180. - OutputAngle);
		};
		*/
		//Add the result to the array
		OutputArray[int(n)] = OutputAngle;
	}
	//pass back the array!
	return OutputArray[0]; 
}

double set_steering_module(double wheelAngle)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1ms);
	return wheelAngle;
}
