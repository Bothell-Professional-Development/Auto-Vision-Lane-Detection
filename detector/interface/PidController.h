#pragma once

//double bezier_calc(double wheelAngle, double laneCenter, double laneAngle, double progressPoint);
double bezier_calc(double progressPoint, double wheelAngle, cv::Point &PointZero, cv::Point &PointOne, cv::Point &PointTwo, cv::Point &PointThree);
double set_steering_module(double wheelAngle);
