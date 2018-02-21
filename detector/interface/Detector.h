#pragma once

#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

#include "ConfigFile.h"
#include "proj.h"

void FrameProcessor(common_lib::ConfigFile& cfgFile, ObjectEvent<InputContainer>& process_input, ObjectEvent<OutputContainer>& process_output, bool &running);

cv::Mat HOGHistogramWithTranspose(const cv::Mat& currentFrame);  //Used for testing. Without transpose is used for training
std::vector<cv::Point> getSVMPrediction(int horizontalStart, int horizontalEnd, cv::Mat &resizedImage, cv::Mat &outputMat, cv::Ptr<cv::ml::SVM> &svm);
void plotLanePoints(cv::Mat &resizedImage, std::vector<cv::Point> &leftLaneUnfilteredPoints, std::vector<cv::Point> &rightLaneUnfilteredPoints);
void plotLanePoints2(cv::Mat &resizedImage, std::vector<cv::Point> &leftLaneUnfilteredPoints, std::vector<cv::Point> &rightLaneUnfilteredPoints);
std::vector<float> findBestFittingCurve(std::vector<cv::Point> &lanePoints);
const std::vector<cv::Point> filterLanePoints(const std::vector<cv::Point>& unfilteredPoints,
	const cv::Point nearLineStart,
	const cv::Point nearLineEnd,
	const cv::Point farLineStart,
	const cv::Point farLineEnd);
float euclideanDist(cv::Point& p, cv::Point& q);
float distanceToLine(cv::Point line_start, cv::Point line_end, cv::Point point);
cv::Point findCentroidOfLaneArea(int rowStart, int columnStart, cv::Mat &sampleBox);
bool linesIntersection(cv::Point o1, cv::Point p1, cv::Point o2, cv::Point p2, cv::Point &r);
int getCenterOfLanes(cv::Point leftLaneStartPoint, cv::Point leftLaneEndPoint, cv::Point rightLaneStartPoint, cv::Point rightLaneEndPoint);
cv::Point getPointVectorAverage(std::vector<cv::Point> &pointV);

bool sameSign(const int32_t& x, const int32_t& y);
bool findLineLineIntersection(const int32_t& x0, const int32_t& y0,
	const int32_t& x1, const int32_t& y1,
	const int32_t& x2, const int32_t& y2,
	const int32_t& x3, const int32_t& y3,
	int32_t& xOut, int32_t& yOut);

void filterLanesBeforeInsertingInHistory(cv::Point &startPoint, cv::Point &endPoint, std::vector<cv::Point> &startHistoryV, std::vector<cv::Point> &endHistoryV, bool isLeft);