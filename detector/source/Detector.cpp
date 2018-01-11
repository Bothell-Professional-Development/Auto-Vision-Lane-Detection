#include "Detector.h"

#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>

#include "PidController.h"
#include "generated.h"
#include "proj.h"

#include <iostream> //for make_pair
#include <fstream> //for ofstream
#include <utility>
#include <functional>

#include <csignal>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cmath>




float imageResizeFactor = 0.5;
const int32_t HORIZONTAL_RESOLUTION = 1920 * imageResizeFactor;
const int32_t VERTICAL_RESOLUTION = 1080 * imageResizeFactor;
const int32_t BOX_WIDTH = 30 * imageResizeFactor;
const int32_t BOX_HEIGHT = 30 * imageResizeFactor;

const int32_t  VERTICAL_REGION_UPPER = 600 * imageResizeFactor;
const int32_t  VERTICAL_REGION_LOWER = 800 * imageResizeFactor;
const int32_t  HORIZONTAL_REGION_LEFT = 600 * imageResizeFactor;
const int32_t  HORIZONTAL_REGION_RIGHT = 1400 * imageResizeFactor;
const int32_t  HORIZONTAL_CENTER = 960 * imageResizeFactor;
const int32_t AVERAGING_WINDOW_SIZE = 30;
int dynamicCenterOfLanesXval = HORIZONTAL_CENTER;
std::vector<cv::Point> upperCenterHistoryV, lowerCenterHistoryV;
std::vector<cv::Point> leftLaneStartHistoryV, leftLaneEndHistoryV;
std::vector<cv::Point> rightLaneStartHistoryV, rightLaneEndHistoryV;

const cv::Point IDEAL_LEFT_LANE_MARKER_START = cv::Point(720 * imageResizeFactor, VERTICAL_REGION_LOWER);
const cv::Point IDEAL_LEFT_LANE_MARKER_END = cv::Point(995 * imageResizeFactor, VERTICAL_REGION_UPPER);
const cv::Point IDEAL_RIGHT_LANE_MARKER_START = cv::Point(1280 * imageResizeFactor, VERTICAL_REGION_LOWER);
const cv::Point IDEAL_RIGHT_LANE_MARKER_END = cv::Point(1005 * imageResizeFactor, VERTICAL_REGION_UPPER);

const cv::Point ADJACENT_LEFT_LANE_MARKER_START = cv::Point(HORIZONTAL_REGION_LEFT, 718 * imageResizeFactor);
const cv::Point ADJACENT_LEFT_LANE_MARKER_END = cv::Point(985 * imageResizeFactor, VERTICAL_REGION_UPPER);
const cv::Point ADJACENT_RIGHT_LANE_MARKER_START = cv::Point(HORIZONTAL_REGION_RIGHT, 718 * imageResizeFactor);
const cv::Point ADJACENT_RIGHT_LANE_MARKER_END = cv::Point(1015 * imageResizeFactor, VERTICAL_REGION_UPPER);

const int pointDistanceFromLaneThreshold = 20 * imageResizeFactor;




void FrameProcessor(common_lib::ConfigFile& cfgFile, ObjectEvent<InputContainer>& process_input, ObjectEvent<OutputContainer>& process_output)
{
	using namespace std::chrono_literals;

	cv::Ptr<cv::ml::SVM> svmLeft = cv::ml::SVM::create();
	svmLeft = cv::ml::SVM::load(cfgFile.readValueOrDefault("SVM_LEFT_MODEL", ""));

	cv::Ptr<cv::ml::SVM> svmRight = cv::ml::SVM::create();
	svmRight = cv::ml::SVM::load(cfgFile.readValueOrDefault("SVM_RIGHT_MODEL", ""));

	if (svmLeft->empty() || svmRight->empty())
	{
		std::cout << "SVM file corrupted" << std::endl;
		exit(-1);
	}

	std::vector<cv::Point> leftLaneUnfilteredPoints, rightLaneUnfilteredPoints;
	std::vector<cv::Point> leftLaneFilteredPoints, rightLaneFilteredPoints;
	std::vector<float> leftLaneLine, rightLaneLine;
	cv::Point leftLaneStartPoint, leftLaneEndPoint, rightLaneStartPoint, rightLaneEndPoint;

	int twoWayProjectionDistanceForLine = 300 * imageResizeFactor;

	//These are just for calculating the fps ratio


	clock_t oldTime = clock();
	clock_t newTime;
	int lastKnownCenterX = HORIZONTAL_CENTER;

	InputContainer input;
	OutputContainer output;
	cv::Mat resizedImage;
	//while (gRunning && detection_input.WaitGetReset<double, std::milli>(1500ms, input))
	while (gRunning && process_input.WaitGetReset(input))
	{
		if (!gRunning)
		{
			std::cout << "gRunning stopped\n";
			break;
		}

		//Get the processing time per frame
		newTime = clock();
		output.frameCounter++;

		if (output.frameCounter % FRAME_SKIP == 0)
		{
			output.timePF = (newTime - oldTime) / (CLOCKS_PER_SEC / 1000);
			oldTime = newTime;
			resize(input.frame, resizedImage, cv::Size(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION));
			//resizedImage = frame.clone(); //First try with full resolution
			cvtColor(resizedImage, resizedImage, CV_BGR2GRAY);
			resizedImage.convertTo(resizedImage, CV_32FC1); //Grayscale
			resizedImage /= 255;

			//Only for visualization purposes. Should be removed from final product
			resizedImage.convertTo(output.outputMat, CV_32FC3);
			cvtColor(output.outputMat, output.outputMat, CV_GRAY2BGR);

			// TESTING: Mark the Region Of Interest ROI
			cv::line(output.outputMat, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER), cv::Scalar(0, 255, 0), 2, 8, 0);
			cv::line(output.outputMat, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
			cv::line(output.outputMat, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
			cv::line(output.outputMat, cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
			//imshow("ROI", resizedImage);
			//waitKey(0);


			leftLaneUnfilteredPoints = getSVMPrediction(HORIZONTAL_REGION_LEFT, dynamicCenterOfLanesXval, resizedImage, output.outputMat, svmLeft);
			rightLaneUnfilteredPoints = getSVMPrediction(dynamicCenterOfLanesXval, HORIZONTAL_REGION_RIGHT, resizedImage, output.outputMat, svmRight);
			output.totalPointsFound += leftLaneUnfilteredPoints.size() + rightLaneUnfilteredPoints.size();

			//Find a crude line first
			if (leftLaneUnfilteredPoints.size() > 2)
			{
				/*leftLaneLine = findBestFittingCurve(leftLaneUnfilteredPoints);
				leftLaneStartPoint = Point(leftLaneLine[2] - leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] - leftLaneLine[1] * twoWayProjectionDistanceForLine);
				leftLaneEndPoint = Point(leftLaneLine[2] + leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] + leftLaneLine[1] * twoWayProjectionDistanceForLine);
				//Filter out the outliers
				leftLaneFilteredPoints = filterLanePoints(leftLaneUnfilteredPoints, leftLaneStartPoint, leftLaneEndPoint);*/
				//test new filter
				leftLaneFilteredPoints = filterLanePoints(leftLaneUnfilteredPoints,
					IDEAL_LEFT_LANE_MARKER_START,
					IDEAL_LEFT_LANE_MARKER_END,
					ADJACENT_LEFT_LANE_MARKER_START,
					ADJACENT_LEFT_LANE_MARKER_END);
				//Refine the line
				if (leftLaneFilteredPoints.size() > 2)
				{
					leftLaneLine = findBestFittingCurve(leftLaneFilteredPoints);
					leftLaneStartPoint = cv::Point(leftLaneLine[2] - leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] - leftLaneLine[1] * twoWayProjectionDistanceForLine);
					leftLaneEndPoint = cv::Point(leftLaneLine[2] + leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] + leftLaneLine[1] * twoWayProjectionDistanceForLine);
				}
			}

			if (rightLaneUnfilteredPoints.size() > 2)
			{
				/*rightLaneLine = findBestFittingCurve(rightLaneUnfilteredPoints);
				rightLaneStartPoint = Point(rightLaneLine[2] - rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] - rightLaneLine[1] * twoWayProjectionDistanceForLine);
				rightLaneEndPoint = Point(rightLaneLine[2] + rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] + rightLaneLine[1] * twoWayProjectionDistanceForLine);
				//Filter out the outliers
				rightLaneFilteredPoints = filterLanePoints(rightLaneUnfilteredPoints, rightLaneStartPoint, rightLaneEndPoint);*/
				rightLaneFilteredPoints = filterLanePoints(rightLaneUnfilteredPoints,
					IDEAL_RIGHT_LANE_MARKER_START,
					IDEAL_RIGHT_LANE_MARKER_END,
					ADJACENT_RIGHT_LANE_MARKER_START,
					ADJACENT_RIGHT_LANE_MARKER_END);

				if (rightLaneFilteredPoints.size() > 2)
				{
					rightLaneLine = findBestFittingCurve(rightLaneFilteredPoints);
					rightLaneStartPoint = cv::Point(rightLaneLine[2] - rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] - rightLaneLine[1] * twoWayProjectionDistanceForLine);
					rightLaneEndPoint = cv::Point(rightLaneLine[2] + rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] + rightLaneLine[1] * twoWayProjectionDistanceForLine);
				}
			}

			//Get the average of the lane points from the last frames
			leftLaneStartHistoryV.push_back(leftLaneStartPoint);
			leftLaneEndHistoryV.push_back(leftLaneEndPoint);
			rightLaneStartHistoryV.push_back(rightLaneStartPoint);
			rightLaneEndHistoryV.push_back(rightLaneEndPoint);
			leftLaneStartPoint = getPointVectorAverage(leftLaneStartHistoryV);
			leftLaneEndPoint = getPointVectorAverage(leftLaneEndHistoryV);
			rightLaneStartPoint = getPointVectorAverage(rightLaneStartHistoryV);
			rightLaneEndPoint = getPointVectorAverage(rightLaneEndHistoryV);
			line(output.outputMat, leftLaneStartPoint, leftLaneEndPoint, cv::Scalar(0, 0, 1), 2, 8);
			line(output.outputMat, rightLaneStartPoint, rightLaneEndPoint, cv::Scalar(1, 0, 0), 2, 8);
			//debug - approximate idealized lane margins
			line(output.outputMat, IDEAL_LEFT_LANE_MARKER_START, IDEAL_LEFT_LANE_MARKER_END, cv::Scalar(0, 127, 127), 1, 8);
			line(output.outputMat, IDEAL_RIGHT_LANE_MARKER_START, IDEAL_RIGHT_LANE_MARKER_END, cv::Scalar(0, 127, 127), 1, 8);
			line(output.outputMat, ADJACENT_LEFT_LANE_MARKER_START, ADJACENT_LEFT_LANE_MARKER_END, cv::Scalar(0, 127, 127), 1, 8);
			line(output.outputMat, ADJACENT_RIGHT_LANE_MARKER_START, ADJACENT_RIGHT_LANE_MARKER_END, cv::Scalar(0, 127, 127), 1, 8);

			//debug - predicted lane center
			int laneCenterX = HORIZONTAL_CENTER;
			int leftHorizonIntersectionX = HORIZONTAL_CENTER;
			int rightHorizonIntersectionX = HORIZONTAL_CENTER;
			int horizonY = VERTICAL_REGION_UPPER + 50;

			int leftIntersectionX = 0;
			int rightIntersectionX = 0;
			int intersectionY = 0;

			if (leftLaneFilteredPoints.size() > 2)
			{
				findLineLineIntersection(leftLaneStartPoint.x,
					leftLaneStartPoint.y,
					leftLaneEndPoint.x,
					leftLaneEndPoint.y,
					0,
					VERTICAL_REGION_LOWER,
					HORIZONTAL_RESOLUTION,
					VERTICAL_REGION_LOWER,
					leftIntersectionX,
					intersectionY);

				findLineLineIntersection(leftLaneStartPoint.x,
					leftLaneStartPoint.y,
					leftLaneEndPoint.x,
					leftLaneEndPoint.y,
					0,
					horizonY,
					HORIZONTAL_RESOLUTION,
					horizonY,
					leftHorizonIntersectionX,
					intersectionY);
			}
			if (rightLaneFilteredPoints.size() > 2)
			{
				findLineLineIntersection(rightLaneStartPoint.x,
					rightLaneStartPoint.y,
					rightLaneEndPoint.x,
					rightLaneEndPoint.y,
					0,
					VERTICAL_REGION_LOWER,
					HORIZONTAL_RESOLUTION,
					VERTICAL_REGION_LOWER,
					rightIntersectionX,
					intersectionY);

				findLineLineIntersection(rightLaneStartPoint.x,
					rightLaneStartPoint.y,
					rightLaneEndPoint.x,
					rightLaneEndPoint.y,
					0,
					horizonY,
					HORIZONTAL_RESOLUTION,
					horizonY,
					rightHorizonIntersectionX,
					intersectionY);
			}

			//average left and right lane intersections with RoI box
			if (leftLaneFilteredPoints.size() > 2 && rightLaneFilteredPoints.size() > 2 &&
				leftIntersectionX > 0 && rightIntersectionX > 0)
			{
				laneCenterX = (((leftIntersectionX + rightIntersectionX) / 2) + lastKnownCenterX) / 2;
				lastKnownCenterX = laneCenterX;
			}
			//estimate center from left marker
			else if (leftLaneFilteredPoints.size() > 2 && leftIntersectionX > 0)
			{
				laneCenterX = ((leftIntersectionX + (IDEAL_RIGHT_LANE_MARKER_START.x - IDEAL_LEFT_LANE_MARKER_START.x) / 2) + lastKnownCenterX) / 2;
				lastKnownCenterX = laneCenterX;
			}
			//estimate center from right marker
			else if (rightLaneFilteredPoints.size() > 2 && rightIntersectionX > 0)
			{
				laneCenterX = ((rightIntersectionX - (IDEAL_RIGHT_LANE_MARKER_START.x - IDEAL_LEFT_LANE_MARKER_START.x) / 2) + lastKnownCenterX) / 2;
				lastKnownCenterX = laneCenterX;
			}
			//last known good?
			else
			{
				laneCenterX = lastKnownCenterX;
			}

			std::ostringstream str;
			str << "Out: " << laneCenterX - ((IDEAL_LEFT_LANE_MARKER_START.x + IDEAL_RIGHT_LANE_MARKER_START.x) / 2);

			putText(output.outputMat, str.str(), cvPoint(HORIZONTAL_CENTER, VERTICAL_REGION_LOWER + 20),
				cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);


			//line(outputMat, cv::Point((IDEAL_LEFT_LANE_MARKER_START.x + IDEAL_RIGHT_LANE_MARKER_START.x) / 2, VERTICAL_REGION_UPPER),
			//	cv::Point(laneCenterX, VERTICAL_REGION_LOWER),
			//	Scalar(0, 127, 0),
			//	1,
			//	8);
			line(output.outputMat, cv::Point((leftHorizonIntersectionX + rightHorizonIntersectionX) / 2, horizonY),
				cv::Point(laneCenterX, VERTICAL_REGION_LOWER),
				cv::Scalar(0, 127, 0),
				1,
				8);

			//plotLanePoints(outputMat, leftLaneFilteredPoints, rightLaneFilteredPoints);
			dynamicCenterOfLanesXval = getCenterOfLanes(leftLaneStartPoint, leftLaneEndPoint, rightLaneStartPoint, rightLaneEndPoint);
			output.BezPointThree = getPointVectorAverage(upperCenterHistoryV);
			output.BezPointTwo = getPointVectorAverage(lowerCenterHistoryV);
			circle(output.outputMat, output.BezPointThree, 5, cv::Scalar(124, 200, 10), 2, 8, 0);
			circle(output.outputMat, output.BezPointTwo, 5, cv::Scalar(124, 200, 10), 2, 8, 0);
			//line(outputMat, cv::Point(dynamicCenterOfLanesXval, 0), cv::Point(dynamicCenterOfLanesXval, VERTICAL_REGION_LOWER),Scalar(0, 0, 1.0), 2, 8, 0);
			//outputMat *= 255;
			//outputMat.convertTo(outputMat, CV_8UC3); 

			//PID Controller visualization:
			output.BezPointZero = { HORIZONTAL_RESOLUTION / 2, VERTICAL_RESOLUTION };
			//Setting the intermediary points.  Eventually this will need to be updated to take into account  the angle of the wheels
			output.BezPointOne = { output.BezPointZero.x, output.BezPointTwo.y };

			//some more setup for the actual drawing of lines.
			cv::Point2f BezLineStart = { 0,0 };
			cv::Point2f BezLineEnd = { 0,0 };
			//Time to draw the best fit curve.  TODO: Make it adjustable how many line segments we want to display (because changing it manually is a pain right now)
			for (double n = 0; n < 10; n++) //important!  n NEEDS to be a double for the math to behave properly.
			{
				if (n == 0)
				{
					BezLineStart =
						pow(1 - (n / 10), 3) * output.BezPointZero
						+ 3 * pow(1 - (n / 10), 2) * (n / 10) * output.BezPointOne
						+ 3 * (1 - (n / 10)) * pow((n / 10), 2) * output.BezPointTwo
						+ pow((n / 10), 3) * output.BezPointThree;
				}
				else
				{
					BezLineStart = BezLineEnd;
				}

				BezLineEnd = pow(1 - ((n + 1) / 10), 3) * output.BezPointZero
					+ 3 * pow(1 - ((n + 1) / 10), 2) * ((n + 1) / 10) * output.BezPointOne
					+ 3 * (1 - ((n + 1) / 10)) * pow(((n + 1) / 10), 2) * output.BezPointTwo
					+ pow(((n + 1) / 10), 3) * output.BezPointThree;
				cv::line(output.outputMat, BezLineStart, BezLineEnd, cv::Scalar(255, 0, 5 + (n * 50)), 2, 8, 0);
				//some debug code for stepping through to see the curve draw bit by bit:
				//cv::imshow("test name", outputMat);
				//cv::waitKey(0);
			}
			//end of PID Controller visualization

			//outputMat *= 255;
			//outputMat.convertTo(outputMat, CV_8UC3); 
			//imwrite("D:/WorkFolder/LaneDetectionTrainingData/SVM_Results.jpg", outputMat);
			//return -1;
		}

		process_output.SetAndSignal(output);
	}

	std::cout << "FrameProcessor ended\n";
}



cv::Mat HOGHistogramWithTranspose(const cv::Mat& currentFrame)
{
	//Mat newFrame;
	//currentFrame.convertTo(newFrame, CV_32FC1);
	//newFrame /= 255;

	cv::Mat inputM = cv::Mat::zeros(currentFrame.size(), currentFrame.type());
	int i, j, k, l;

	cv::Mat hist = cv::Mat::zeros(1, 32, CV_32FC1);  ////32 bins	
	std::stringstream bin_pat;

	//Find the upper left and lower right corner of the blob's bounding box

	int upper_y = 0;
	int left_x = 0;
	int lower_y = currentFrame.rows;
	int right_x = currentFrame.cols;

	double step = 360.0 / hist.cols;

	for (i = upper_y + 1; i<lower_y - 1; i++)
	{
		for (j = left_x + 1; j<right_x - 1; j++)
		{

			inputM.at<float>(i, j) = currentFrame.at<float>(i, j);

			double xDiff = currentFrame.at<float>(i + 1, j) - currentFrame.at<float>(i - 1, j);
			double yDiff = currentFrame.at<float>(i, j + 1) - currentFrame.at<float>(i, j - 1);

			double magnitude = sqrt(xDiff*xDiff + yDiff*yDiff);
			double angle = atan2(xDiff, yDiff) * 180 / CV_PI + 180.0;  //in degrees, atan2 is from -p to +p

			double result = angle / ((360.0 / hist.cols));
			int res = (int)result;
			if (res == hist.cols) res--;
			hist.at<float>(0, res) += magnitude;

		}
	}

	//cv::imshow("input", inputM);
	//cv::waitKey(1);	
	//showHistogram(hist);

	return hist;
}


void showHistogram(const cv::Mat& histogram)
{
	std::stringstream histrogramNameSS;
	histrogramNameSS << "HOG" << " Histogram";
	std::string windowLabel = histrogramNameSS.str();

	int maxValue = 0;

	for (int i = 0; i<histogram.rows; i++)
		if (histogram.at<float>(i, 0) > maxValue) maxValue = histogram.at<float>(i, 0);

	cv::Mat canvas = cv::Mat::zeros(240, 320, CV_8UC1);
	int newWidth = (canvas.cols / (double)histogram.rows);
	if (maxValue != 0)
	{
		for (int i = 0; i<histogram.rows; i++)
		{
			cv::Scalar clr;
			if (i % 2 == 0) clr = cv::Scalar(255, 255, 255);
			else clr = cv::Scalar(150, 150, 150);

			cv::Rect newRect;
			newRect.x = i*newWidth;
			newRect.y = canvas.rows*(1.0 - histogram.at<float>(i, 0) / maxValue);
			newRect.width = newWidth;
			newRect.height = canvas.rows - newRect.y;
			cv::rectangle(canvas, newRect, clr, CV_FILLED, CV_AA);
		}
	}

	cv::imshow(windowLabel, canvas);
	cv::waitKey(1);
}

std::vector<cv::Point> getSVMPrediction(int horizontalStart, int horizontalEnd, cv::Mat &resizedImage, cv::Mat &outputMat, cv::Ptr<cv::ml::SVM> &svm)
{
	cv::Mat sampleBox = cv::Mat(BOX_WIDTH, BOX_HEIGHT, CV_32FC1);
	cv::Mat histogramOfFeature, transposedMat; //TODO: return the histogram transposed from the function
	int responseSVM = 0;
	std::vector<cv::Point> unfilteredLanePoints;
	cv::Point centerOfLaneBox;
	int cornerOffset = ceil((float)(VERTICAL_REGION_LOWER - VERTICAL_REGION_UPPER) / BOX_HEIGHT);
	int leftCornerOffset, rightCornerOffset;
	if (horizontalEnd == dynamicCenterOfLanesXval) //We want to cut the upper left corner
	{
		leftCornerOffset = 1;
		rightCornerOffset = 0;
	}
	else //We want to cut the upper right corner
	{
		rightCornerOffset = -1;
		leftCornerOffset = 0;
	}

	for (int r = VERTICAL_REGION_UPPER; r < VERTICAL_REGION_LOWER; r += BOX_HEIGHT, cornerOffset--)
	{
		for (int c = horizontalStart + cornerOffset*leftCornerOffset*BOX_WIDTH; c < horizontalEnd + cornerOffset*rightCornerOffset*BOX_WIDTH; c += BOX_WIDTH)
		{
			{
				sampleBox = cv::Mat(resizedImage, cv::Range(r, r + BOX_HEIGHT), cv::Range(c, c + BOX_WIDTH));

				histogramOfFeature = HOGHistogramWithTranspose(sampleBox);
				responseSVM = svm->predict(histogramOfFeature);

				//if (responseSVM == 1)
				//{
				//	//Mark the cell with a red border
				//	cv::line(outputMat, cv::Point(c + 1, r + 1), cv::Point(c - 1 + BOX_WIDTH, r + 1), cv::Scalar(0, 0, 255), 1, 8, 0);
				//	cv::line(outputMat, cv::Point(c - 1 + BOX_WIDTH, r + 1), cv::Point(c - 1 + BOX_WIDTH, r - 1 + BOX_HEIGHT), cv::Scalar(0, 0, 255), 1, 8, 0);
				//	cv::line(outputMat, cv::Point(c - 1 + BOX_WIDTH, r - 1 + BOX_HEIGHT), cv::Point(c + 1, r - 1 + BOX_HEIGHT), cv::Scalar(0, 0, 255), 1, 8, 0);
				//	cv::line(outputMat, cv::Point(c + 1, r - 1 + BOX_HEIGHT), cv::Point(c + 1, r + 1), cv::Scalar(0, 0, 255), 1, 8, 0);
				//}
				//if (responseSVM == -1)
				//{
				//	//Mark the cell with a red border
				//	cv::line(outputMat, cv::Point(c + 1, r + 1), cv::Point(c - 1 + BOX_WIDTH, r + 1), cv::Scalar(255, 0, 0), 1, 8, 0);
				//	cv::line(outputMat, cv::Point(c - 1 + BOX_WIDTH, r + 1), cv::Point(c - 1 + BOX_WIDTH, r - 1 + BOX_HEIGHT), cv::Scalar(255, 0, 0), 1, 8, 0);
				//	cv::line(outputMat, cv::Point(c - 1 + BOX_WIDTH, r - 1 + BOX_HEIGHT), cv::Point(c + 1, r - 1 + BOX_HEIGHT), cv::Scalar(255, 0, 0), 1, 8, 0);
				//	cv::line(outputMat, cv::Point(c + 1, r - 1 + BOX_HEIGHT), cv::Point(c + 1, r + 1), cv::Scalar(255, 0, 0), 1, 8, 0);
				//}

				if (responseSVM == 1)
				{
					//Find centroid of brighter part of image
					centerOfLaneBox = findCentroidOfLaneArea(r, c, sampleBox);
					//centerOfLaneBox = Point(c + BOX_WIDTH / 2, r + BOX_HEIGHT / 2);
					unfilteredLanePoints.push_back(centerOfLaneBox);

				}
				//imshow("test", outputMat);
				//waitKey(1);
			}
		}
	}

	return unfilteredLanePoints;
}

void plotLanePoints(cv::Mat &resizedImage, std::vector<cv::Point> &leftLaneUnfilteredPoints, std::vector<cv::Point> &rightLaneUnfilteredPoints)
{
	for (int i = 0; i < leftLaneUnfilteredPoints.size(); i++)
	{
		circle(resizedImage, leftLaneUnfilteredPoints[i], 3, cv::Scalar(0, 0, 1.0), 2, 8, 0);
	}

	for (int i = 0; i < rightLaneUnfilteredPoints.size(); i++)
	{
		circle(resizedImage, rightLaneUnfilteredPoints[i], 3, cv::Scalar(1.0, 0, 0), 2, 8, 0);
	}
}

std::vector<float> findBestFittingCurve(std::vector<cv::Point> &lanePoints)
{
	std::vector<float> laneLine;
	cv::fitLine(lanePoints, laneLine, CV_DIST_L2, 0, 0.01, 0.01);
	return laneLine;
}

std::vector<cv::Point> filterLanePoints(std::vector<cv::Point> &unfilteredPoints, cv::Point line_start, cv::Point line_end)
{
	std::vector<float> distancesToLineV;
	std::vector<cv::Point> filteredPoints;
	for (int i = 0; i < unfilteredPoints.size(); i++)
	{
		distancesToLineV.push_back(distanceToLine(line_start, line_end, unfilteredPoints[i]));
	}
	//Remove outlier normalize by angle (left and right) and then use the 2*sd
	//Use just a threshold for now

	for (int i = 0; i < unfilteredPoints.size(); i++)
	{
		if (distancesToLineV[i] < pointDistanceFromLaneThreshold)
		{
			filteredPoints.push_back(unfilteredPoints[i]);
		}
	}
	return filteredPoints;
}

const std::vector<cv::Point> filterLanePoints(const std::vector<cv::Point>& unfilteredPoints,
	const cv::Point nearLineStart,
	const cv::Point nearLineEnd,
	const cv::Point farLineStart,
	const cv::Point farLineEnd)
{
	std::vector<cv::Point> closeToNearLinePoints;
	std::vector<float> distancesToNearLine;
	float totalDist = 0.0;

	for (int i = 0; i < unfilteredPoints.size(); ++i)
	{
		float distToNearLine = distanceToLine(nearLineStart, nearLineEnd, unfilteredPoints[i]);
		float distToFarLine = distanceToLine(farLineStart, farLineEnd, unfilteredPoints[i]);

		if (distToNearLine < distToFarLine)
		{
			closeToNearLinePoints.push_back(unfilteredPoints[i]);
			distancesToNearLine.push_back(distToNearLine);
			totalDist += distToNearLine;
		}
	}

	std::vector<cv::Point> filteredPoints;
	float avgDistToNear = totalDist / distancesToNearLine.size();

	for (int i = 0; i < closeToNearLinePoints.size(); ++i)
	{
		if (distancesToNearLine[i] < avgDistToNear * 1.05)
		{
			filteredPoints.push_back(closeToNearLinePoints[i]);
		}
	}

	return filteredPoints;
}

float distanceToLine(cv::Point line_start, cv::Point line_end, cv::Point point)
{
	float normalLength = hypot(line_end.x - line_start.x, line_end.y - line_start.y);
	float distance = (double)((point.x - line_start.x) * (line_end.y - line_start.y) - (point.y - line_start.y) * (line_end.x - line_start.x)) / normalLength;
	return abs(distance);
}

float euclideanDist(cv::Point& p, cv::Point& q)
{
	cv::Point diff = p - q;
	return cv::sqrt((float)(diff.x*diff.x + diff.y*diff.y));
}

cv::Point findCentroidOfLaneArea(int rowStart, int columnStart, cv::Mat &sampleBox)
{
	sampleBox *= 255;
	sampleBox.convertTo(sampleBox, CV_8UC1);

	double otsu = 100;
	cv::Mat otsuMask(sampleBox.size(), sampleBox.type());
	threshold(sampleBox, otsuMask, otsu, 255, cv::THRESH_OTSU);
	//adaptiveThreshold(sampleBox, otsuMask, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 15, 5);

	//Find contours of lane
	std::vector<cv::Vec4i> hierarchy;
	std::vector<std::vector<cv::Point>> contoursV;
	cv::findContours(otsuMask, contoursV, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//Find largest contour
	int largestContourSize = 0;
	int largestContourIndex = 0;
	for (int i = 0; i < contoursV.size(); i++)
	{
		if (contoursV[i].size() > largestContourSize)
		{
			largestContourSize = contoursV[i].size();
			largestContourIndex = i;
		}
	}

	////Draw the largest contour
	//otsuMask *= 0;
	//std::vector < std::vector<cv::Point>> finalBlob;
	//finalBlob.push_back(contoursV[largestContourIndex]);
	//cv::drawContours(otsuMask, finalBlob, -1, Scalar(255), CV_FILLED, 8);
	//imshow("after", otsuMask);
	//waitKey(0);

	//Find the centroid of the largest contour
	cv::Moments mu = moments(contoursV[largestContourIndex], false);
	return cv::Point((int)(mu.m10 / mu.m00) + columnStart, (int)(mu.m01 / mu.m00) + rowStart);
}

bool findLineLineIntersection(const int32_t& x0, const int32_t& y0,
	const int32_t& x1, const int32_t& y1,
	const int32_t& x2, const int32_t& y2,
	const int32_t& x3, const int32_t& y3,
	int32_t& xOut, int32_t& yOut)
{
	int32_t a0 = y1 - y0;
	int32_t b0 = x0 - x1;
	int32_t c0 = x1 * y0 - x0 * y1;

	int32_t r2 = a0 * x2 + b0 * y2 + c0;
	int32_t r3 = a0 * x3 + b0 * y3 + c0;

	if (r2 != 0 && r3 != 0 && sameSign(r2, r3))
	{
		return false;
	}

	int32_t a1 = y3 - y2;
	int32_t b1 = x2 - x3;
	int32_t c1 = x3 * y2 - x2 * y3;

	int32_t r0 = a1 * x0 + b1 * y0 + c1;
	int32_t r1 = a1 * x1 + b1 * y1 + c1;

	if (r0 != 0 && r2 != 0 && sameSign(r0, r1))
	{
		return false;
	}

	int32_t denom = a0 * b1 - a1 * b0;

	if (denom == 0)
	{
		return false;
	}

	int32_t offset = denom < 0 ? -denom / 2 : denom / 2;

	int32_t num = b0 * c1 - b1 * c0;
	xOut = (num < 0 ? num - offset : num + offset) / denom;

	num = a1 * c0 - a0 * c1;
	yOut = (num < 0 ? num - offset : num + offset) / denom;

	return true;
}

bool sameSign(const int32_t& x, const int32_t& y)
{
	return (x >= 0) ^ (y < 0);
}

bool linesIntersection(cv::Point o1, cv::Point p1, cv::Point o2, cv::Point p2, cv::Point &r)
{
	cv::Point x = o2 - o1;
	cv::Point d1 = p1 - o1;
	cv::Point d2 = p2 - o2;

	float cross = d1.x*d2.y - d1.y*d2.x;
	if (std::abs(cross) < /*EPS*/1e-8)
		return false;

	double t1 = (x.x * d2.y - x.y * d2.x) / cross;
	r = o1 + d1 * t1;
	return true;
}

int getCenterOfLanes(cv::Point leftLaneStartPoint, cv::Point leftLaneEndPoint, cv::Point rightLaneStartPoint, cv::Point rightLaneEndPoint)
{
	cv::Point upperLeftIntersection;
	cv::Point upperRightIntersection;
	cv::Point lowerLeftIntersection;
	cv::Point lowerRightIntersection;

	bool centerFound = true;
	centerFound = linesIntersection(cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER),
		leftLaneStartPoint, leftLaneEndPoint, upperLeftIntersection);
	centerFound = linesIntersection(cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER),
		rightLaneStartPoint, rightLaneEndPoint, upperRightIntersection);
	centerFound = linesIntersection(cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER),
		leftLaneStartPoint, leftLaneEndPoint, lowerLeftIntersection);
	centerFound = linesIntersection(cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER),
		rightLaneStartPoint, rightLaneEndPoint, lowerRightIntersection);
	if (centerFound)
	{
		int upperCenter, lowerCenter;
		upperCenter = (int)(upperRightIntersection.x - upperLeftIntersection.x) / 2 + upperLeftIntersection.x;
		lowerCenter = (int)(lowerRightIntersection.x - lowerLeftIntersection.x) / 2 + lowerLeftIntersection.x;
		upperCenterHistoryV.push_back(cv::Point(upperCenter, VERTICAL_REGION_UPPER));
		lowerCenterHistoryV.push_back(cv::Point(lowerCenter, VERTICAL_REGION_LOWER));
		int center = (int)(upperCenter + lowerCenter) / 2;
		if (center > HORIZONTAL_REGION_LEFT && center < HORIZONTAL_REGION_RIGHT) {
			return center;
		}
		else
			return HORIZONTAL_CENTER;
	}
	else
		return HORIZONTAL_CENTER;
}

cv::Point getPointVectorAverage(std::vector<cv::Point> &pointV)
{
	int xAverage = 0;
	int yAverage = 0;
	int i;

	if (pointV.size() < 2)
	{
		return cv::Point(HORIZONTAL_RESOLUTION / 2, VERTICAL_RESOLUTION / 2);
	}

	//Clear out outliers while they are getting into the vector
	if (pointV[pointV.size() - 1].x < 0 || pointV[pointV.size() - 1].x > HORIZONTAL_RESOLUTION ||
		pointV[pointV.size() - 1].y < 0 || pointV[pointV.size() - 1].y > VERTICAL_RESOLUTION)
	{
		pointV.pop_back();
	}
	if (pointV.size() >= AVERAGING_WINDOW_SIZE)
	{
		for (i = 0; i < pointV.size(); i++)
		{
			xAverage += pointV[i].x;
			yAverage += pointV[i].y;
		}
		xAverage = (int)xAverage / i;
		yAverage = (int)yAverage / i;
		pointV.erase(pointV.begin());
		return cv::Point(xAverage, yAverage);
	}
	else
	{
		return pointV[pointV.size() - 1];
	}
}
