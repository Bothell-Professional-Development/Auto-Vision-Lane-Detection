#pragma once
#define BezierMagnitude 80.f
//double bezier_calc(double wheelAngle, double laneCenter, double laneAngle, double progressPoint);
double bezier_calc(float wheelAngle, cv::Point2f &PointZero, cv::Point2f &PointOne, cv::Point2f &PointTwo, cv::Point2f &PointThree, double OutputArray[10]);
double set_steering_module(double wheelAngle);
