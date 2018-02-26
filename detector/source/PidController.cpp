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
	if (PointTwo.y == PointThree.y) {
		PointTwo.y = PointThree.y - BezierMagnitude;
	}
	else
	{
		PointTwo = { PointThree.x + BezierMagnitude * sin(atan2f(PointThree.y - PointTwo.y, PointThree.x - PointTwo.x)) ,
			PointThree.y + BezierMagnitude * cos(atan2f(PointThree.y - PointTwo.y, PointThree.x - PointTwo.x)) };
	}
	
	//setup some points that make math possible
	for (double n = 1; n < steerCommandArraySize; n++) //important!  n NEEDS to be a double for the math to behave properly.
	{
		//This algorithm now uses the tangent of the smoothest curve path to the center line at the given progress point through the curve.
		Result = (3. * pow(1. - (n / steerCommandArraySize), 2.) * (PointOne - PointZero))
			+ (6. * (1. - (n / steerCommandArraySize))*(n / steerCommandArraySize) * (PointTwo - PointOne))
			+ (3. * pow((n / steerCommandArraySize), 2.)*(PointThree - PointTwo));
		//Step three, do the monster math (use the resultant location to calculate an angle to which the wheels should steer)
				//check for divide by 0 problems and set to what the answer should be in that case, otherwise get the real answer
		double OutputAngle;
		if (Result.x == 0.) {
			OutputAngle = CV_PI / 2.;
		}
		else {
			OutputAngle = atan2f(Result.y,Result.x);
		}
		//arctan will output some strange-ish values that make sense for math but not for us.
		//If it needs to go 1 degree to the right, the output is 89 degrees.  If it needs to go 1 degree to the left, the output is -89 degrees
		//this should convert everything to proper values such that needing to steer 1 degree right will make the OutputAngle = 1, steering 1 degree left will make Output Angle = -1, and going straight ahead will make it 0
		//Of course, this outputs radians though.
		if (OutputAngle < 0) {
			OutputAngle = -(CV_PI / 2 + OutputAngle);
		}
		else {
			OutputAngle = (CV_PI / 2 - OutputAngle);
		}
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
