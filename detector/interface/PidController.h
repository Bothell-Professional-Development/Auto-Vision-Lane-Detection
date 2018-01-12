#pragma once

//double bezier_calc(double wheelAngle, double laneCenter, double laneAngle, double progressPoint);
double bezier_calc(float progressPoint, float wheelAngle, cv::Point2f &PointZero, cv::Point2f &PointOne, cv::Point2f &PointTwo, cv::Point2f &PointThree);
double set_steering_module(double wheelAngle);
