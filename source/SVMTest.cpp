// SVMTest.cpp : Defines the entry point for the console application.
//



#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include <iostream> //for make_pair
#include <fstream> //for ofstream
#include <utility>
#include <functional>

#include <Windows.h>
#include <ctime>


using namespace cv;
using namespace cv::ml;
using namespace std;

cv::Mat LBPHistogram(const cv::Mat& currentFrameSegment);
cv::Mat HOGHistogram(const cv::Mat& currentFrame);
cv::Mat HOGHistogramWithTranspose(const cv::Mat& currentFrame);
cv::Mat LPQHistogram(const cv::Mat& currentFrame);
cv::Mat CSLBPHistogram(const cv::Mat& currentFrame);
void showHistogram(const cv::Mat& histogram);
void addTrainingDataAndLabels(string trainingDataPath, float label, vector<float> &labelsV, vector<Mat> &trainingHistogramsV);
void trainSVMClassifier(string trainedSVMfilename, string positiveFilePath, string negativeFilePath, string identifier);
std::vector<cv::Point> getSVMPrediction(int horizontalStart, int horizontalEnd, Mat &resizedImage, Mat &outputMat, cv::Ptr<SVM> &svm);
void plotLanePoints(Mat &resizedImage, vector<Point> &leftLaneUnfilteredPoints, vector<Point> &rightLaneUnfilteredPoints);
std::vector<float> findBestFittingCurve(std::vector<cv::Point> &lanePoints);
std::vector<cv::Point> filterLanePoints(std::vector<cv::Point> &unfilteredPoints, cv::Point line_start, cv::Point line_end);
float euclideanDist(Point& p, Point& q);
float distanceToLine(cv::Point line_start, cv::Point line_end, cv::Point point);
cv::Point findCentroidOfLaneArea(int rowStart, int columnStart, cv::Mat &sampleBox);

float imageResizeFactor = 1;
const int32_t HORIZONTAL_RESOLUTION = 1920 * imageResizeFactor;
const int32_t VERTICAL_RESOLUTION = 1080 * imageResizeFactor;
const int32_t BOX_WIDTH = 30 * imageResizeFactor;
const int32_t BOX_HEIGHT = 30 * imageResizeFactor;
//const string positiveRightFilePath = "D:/WorkFolder/LaneDetectionTrainingData/positiveRight/";
const string positiveRightFilePath = "D:/PDP/SVMTrainingDataTool/positive/right/";
const string negativeRightFilePath = "D:/PDP/SVMTrainingDataTool/negative/right/";
//const string positiveLeftFilePath = "D:/WorkFolder/LaneDetectionTrainingData/positiveLeft/";
//const string negativeFilePath = "D:/WorkFolder/LaneDetectionTrainingData/negative/";
const string positiveLeftFilePath = "D:/PDP/SVMTrainingDataTool/positive/left/";
const string negativeLeftFilePath = "D:/PDP/SVMTrainingDataTool/negative/left/";

#define trainingFilePath(featureString, laneSide) "D:/PDP/SVMTrainingDataTool/SVM_" << featureString << "_" << laneSide << "Lane.xml"
#define histogramFilePath(featureString, identifier) "D:/PDP/SVMTrainingDataTool/HistogramDistanceMatrix_" << featureString << "_" << identifier << ".jpg";

const int32_t  VERTICAL_REGION_UPPER = 600 * imageResizeFactor;
const int32_t  VERTICAL_REGION_LOWER = 800 * imageResizeFactor;
const int32_t  HORIZONTAL_REGION_LEFT = 600 * imageResizeFactor;
const int32_t  HORIZONTAL_REGION_RIGHT = 1400 * imageResizeFactor;
const int32_t  HORIZONTAL_CENTER = 960 * imageResizeFactor;

typedef enum {
	LBP, // is 0
	LPQ, // is 1
	HOG, // is 2
	CSLBP
};
const int featureTypeNum = HOG;  //Choose feature type
const int pointDistanceFromLaneThreshold = 20 * imageResizeFactor;
static const char * EnumStrings[] = { "LBP", "LPQ", "HOG", "CSLBP" };



std::wstring string_to_wstring(const std::string& text) {
	return std::wstring(text.begin(), text.end());
}


int main()
{
	std::cout << "Prog Start" << std::endl;

	stringstream trainedSVMLeftLanefilename;
	stringstream trainedSVMRightLanefilename;
	//trainedSVMLeftLanefilename << "D:/WorkFolder/LaneDetectionTrainingData/SVM_" << EnumStrings[featureTypeNum] << "_LeftLane.xml";
	trainedSVMLeftLanefilename << trainingFilePath(EnumStrings[featureTypeNum], "Left");
	//trainSVMClassifier(trainedSVMLeftLanefilename.str(), positiveLeftFilePath, negativeLeftFilePath, "Left");

	//trainedSVMRightLanefilename << "D:/WorkFolder/LaneDetectionTrainingData/SVM_" << EnumStrings[featureTypeNum] << "_RightLane.xml";
	trainedSVMRightLanefilename << trainingFilePath(EnumStrings[featureTypeNum], "Right");
	//trainSVMClassifier(trainedSVMRightLanefilename.str(), positiveRightFilePath, negativeRightFilePath, "Right");


	//Load video
	cv::VideoCapture cap;
	//cap.open("D:\\WorkFolder\\Auto-Vision-Lane-Detection-build\\vids\\test3.mp4");
	//cap.open("D:/PDP/SVMTrainingDataTool/2.MP4");
	//cap.open("D:\PDP\SVMTrainingDataTool\2.MP4");
	cap.open("D:\\PDP\\SVMTrainingDataTool\\2.MP4");

	if (!cap.isOpened()) // Check for invalid input
	{
		std::cout << "Could not open or find the video file" << std::endl;
		return -1;
	}

	//Create and load already trained SVM classifier
	cv::Ptr<SVM> svmLeft = SVM::create();
	svmLeft = SVM::load(trainedSVMLeftLanefilename.str());

	cv::Ptr<SVM> svmRight = SVM::create();
	svmRight = SVM::load(trainedSVMRightLanefilename.str());
	
	cv::Mat frame, resizedImage, outputMat;

	vector<Point> leftLaneUnfilteredPoints, rightLaneUnfilteredPoints;
	vector<Point> leftLaneFilteredPoints, rightLaneFilteredPoints;
	std::vector<float> leftLaneLine, rightLaneLine;
	cv::Point leftLaneStartPoint, leftLaneEndPoint, rightLaneStartPoint, rightLaneEndPoint;

	int totalPointsFound = 0;
	int twoWayProjectionDistanceForLine = 300 * imageResizeFactor;

	//These are just for calculating the fps ratio
	int timePF = 1;
	float fps;
	clock_t oldTime = clock();
	clock_t newTime;
	int fpsMean = 0;
	int frameCounter = 0;
	while (cap.read(frame) && frame.data != NULL)
	{
		//Get the processing time per frame
		newTime = clock();
		timePF = newTime - oldTime;
		oldTime = newTime;
		fps = 1000 / timePF;
		std::cout << "Processing time/frame " << frameCounter << " : " << timePF << " (" << fps << "fps)" << std::endl;
		fpsMean += fps;
		frameCounter++;

		resize(frame, resizedImage, cv::Size(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION));
		//resizedImage = frame.clone(); //First try with full resolution
		cvtColor(resizedImage, resizedImage, CV_BGR2GRAY);
		resizedImage.convertTo(resizedImage, CV_32FC1); //Grayscale
		resizedImage /= 255;


		//Only for visualization purposes. Should be removed from final product
		resizedImage.convertTo(outputMat, CV_32FC3);
		cvtColor(outputMat, outputMat, CV_GRAY2BGR);

		// TESTING: Mark the Region Of Interest ROI
		cv::line(outputMat, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER), cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(outputMat, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(outputMat, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(outputMat, cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
		//imshow("ROI", resizedImage);
		//waitKey(0);

		leftLaneUnfilteredPoints = getSVMPrediction(HORIZONTAL_REGION_LEFT, HORIZONTAL_CENTER, resizedImage, outputMat, svmLeft);
		rightLaneUnfilteredPoints = getSVMPrediction(HORIZONTAL_CENTER, HORIZONTAL_REGION_RIGHT, resizedImage, outputMat, svmRight);
		totalPointsFound += leftLaneUnfilteredPoints.size() + rightLaneUnfilteredPoints.size();
		
		//Find a crude line first
		if (leftLaneUnfilteredPoints.size() > 2)
		{
			leftLaneLine = findBestFittingCurve(leftLaneUnfilteredPoints);
			leftLaneStartPoint = Point(leftLaneLine[2] - leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] - leftLaneLine[1] * twoWayProjectionDistanceForLine);
			leftLaneEndPoint = Point(leftLaneLine[2] + leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] + leftLaneLine[1] * twoWayProjectionDistanceForLine);
			//Filter out the outliers
			leftLaneFilteredPoints = filterLanePoints(leftLaneUnfilteredPoints, leftLaneStartPoint, leftLaneEndPoint);
			//Refine the line
			if (leftLaneFilteredPoints.size() > 2)
			{
				leftLaneLine = findBestFittingCurve(leftLaneFilteredPoints);
				leftLaneStartPoint = Point(leftLaneLine[2] - leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] - leftLaneLine[1] * twoWayProjectionDistanceForLine);
				leftLaneEndPoint = Point(leftLaneLine[2] + leftLaneLine[0] * twoWayProjectionDistanceForLine, leftLaneLine[3] + leftLaneLine[1] * twoWayProjectionDistanceForLine);
				line(outputMat, leftLaneStartPoint, leftLaneEndPoint, Scalar(0, 0, 1), 1, 8);
			}
		}

		if (rightLaneUnfilteredPoints.size() > 2)
		{
			rightLaneLine = findBestFittingCurve(rightLaneUnfilteredPoints);
			rightLaneStartPoint = Point(rightLaneLine[2] - rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] - rightLaneLine[1] * twoWayProjectionDistanceForLine);
			rightLaneEndPoint = Point(rightLaneLine[2] + rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] + rightLaneLine[1] * twoWayProjectionDistanceForLine);
			//Filter out the outliers
			rightLaneFilteredPoints = filterLanePoints(rightLaneUnfilteredPoints, rightLaneStartPoint, rightLaneEndPoint);
			if (rightLaneFilteredPoints.size() > 2)
			{
				rightLaneLine = findBestFittingCurve(rightLaneFilteredPoints);
				rightLaneStartPoint = Point(rightLaneLine[2] - rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] - rightLaneLine[1] * twoWayProjectionDistanceForLine);
				rightLaneEndPoint = Point(rightLaneLine[2] + rightLaneLine[0] * twoWayProjectionDistanceForLine, rightLaneLine[3] + rightLaneLine[1] * twoWayProjectionDistanceForLine);
				line(outputMat, rightLaneStartPoint, rightLaneEndPoint, Scalar(1, 0, 0), 1, 8);
			}
		}
		plotLanePoints(outputMat, leftLaneFilteredPoints, rightLaneFilteredPoints);
		//outputMat *= 255;
		//outputMat.convertTo(outputMat, CV_8UC3); 
	
		//imwrite("D:/WorkFolder/LaneDetectionTrainingData/SVM_Results.jpg", outputMat);
		//return -1;
		imshow("Classification", outputMat);
		waitKey(1);
	}
	if (frameCounter != 0)
	{
		fpsMean = (int)fpsMean / frameCounter;
	}
	std::cout << "Average fps rate: " << fpsMean << std::endl;
	cout << "Total points found: " << totalPointsFound << endl;
	
	return 0;
}



cv::Mat LBPHistogram(const cv::Mat& currentFrameSegment)
{
	int i, j, k, l;
	cv::Mat hist = cv::Mat::zeros(32, 1, CV_32FC1);  ////32 bins	
	std::stringstream bin_pat;

	//Find the upper left and lower right corner of the blob's bounding box

	for (int i=1; i<currentFrameSegment.rows-3; i++)
	{
		for (int j=1; j<currentFrameSegment.cols-3; j++)
		{
			for (k = (i - 1); k<(i + 2); k++)
			{
				for (l = (j - 1); l<(j + 2); l++)
				{
					if (k == i && l == j) continue;
					else
					{
						if (currentFrameSegment.at<float>(i, j) >= currentFrameSegment.at<float>(k, l))
							bin_pat << 1;
						else bin_pat << 0;
					}
				}
			}

			double result = 0.0;
			int pow = 1;
			for (k = bin_pat.str().length() - 1; k >= 0; --k, pow <<= 1)
				result += (bin_pat.str().at(k) - '0') * pow;

			result /= (255.0 / hist.rows);
			int res = (int)result;
			if (res == hist.rows) res--;
			hist.at<float>(res, 0) += 1.0;

			bin_pat.str("");
			bin_pat.clear();
			
		}
	}

	//showHistogram(hist);

	return hist;
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

	for (i = upper_y+1; i<lower_y-1; i++)
	{
		for (j = left_x+1; j<right_x-1; j++)
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

cv::Mat HOGHistogram(const cv::Mat& currentFrame)
{
	//Mat newFrame;
	//currentFrame.convertTo(newFrame, CV_32FC1);
	//newFrame /= 255;

	cv::Mat inputM = cv::Mat::zeros(currentFrame.size(), currentFrame.type());
	int i, j, k, l;

	cv::Mat hist = cv::Mat::zeros(32, 1, CV_32FC1);  ////32 bins	
	std::stringstream bin_pat;

	//Find the upper left and lower right corner of the blob's bounding box

	int upper_y = 0;
	int left_x = 0;
	int lower_y = currentFrame.rows;
	int right_x = currentFrame.cols;

	double step = 360.0 / hist.rows;

	for (i = upper_y + 1; i<lower_y - 1; i++)
	{
		for (j = left_x + 1; j<right_x - 1; j++)
		{

			inputM.at<float>(i, j) = currentFrame.at<float>(i, j);

			double xDiff = currentFrame.at<float>(i + 1, j) - currentFrame.at<float>(i - 1, j);
			double yDiff = currentFrame.at<float>(i, j + 1) - currentFrame.at<float>(i, j - 1);

			double magnitude = sqrt(xDiff*xDiff + yDiff*yDiff);
			double angle = atan2(xDiff, yDiff) * 180 / CV_PI + 180.0;  //in degrees, atan2 is from -p to +p

			double result = angle / ((360.0 / hist.rows));
			int res = (int)result;
			if (res == hist.rows) res--;
			hist.at<float>(res, 0) += magnitude;

		}
	}

	//cv::imshow("input", inputM);
	//cv::waitKey(1);	
	//showHistogram(hist);

	return hist;
}

cv::Mat LPQHistogram(const cv::Mat& currentFrame)
{


	
	cv::Mat inputM = cv::Mat::zeros(currentFrame.size(), currentFrame.type());
	int i, j, k, l, m;
	

	cv::Mat hist = cv::Mat::zeros(32, 1, CV_32FC1);  ////32 bins	
	std::stringstream bin_pat;

	

	int upper_y = 0;
	int left_x = 0;
	int lower_y = currentFrame.rows;
	int right_x = currentFrame.cols;

	int nr = 7.0;
	double a = 1.0 / nr;

	std::vector<std::pair<double, double>> frequencyV;
	frequencyV.push_back(pair<double, double>(a, 0));
	frequencyV.push_back(pair<double, double>(0, a));
	frequencyV.push_back(pair<double, double>(a, a));
	frequencyV.push_back(pair<double, double>(a, -a));

	for (i = upper_y; i<lower_y; i++)
	{
		for (j = left_x; j<right_x; j++)
		{
			/*std::cout << i << "  " << j << std::endl;
			if (i==145 && j == 148)
			cv::waitKey();*/

				inputM.at<float>(i, j) = currentFrame.at<float>(i, j);

				for (m = 0; m<frequencyV.size(); m++)
				{
					double realVal = 0.0;
					double imagVal = 0.0;
					int xc = 0, yc = 0;
					for (k = i - (int)(nr / 2.0); k<i + (int)(nr / 2.0) + 1; k++)
					{
						for (l = j - (int)(nr / 2.0); l<j + (int)(nr / 2.0) + 1; l++)
						{
							if ((k == i && l == j) || k<0 || l<0 || k>currentFrame.rows - 1 || l>currentFrame.cols - 1)  continue;
							else
							{

								double theta = -2.0*CV_PI*(frequencyV.at(m).first*(k - i) + frequencyV.at(m).second*(l - j));
								double r = currentFrame.at<float>(k, l);

								realVal += r*cos(theta);
								imagVal += r*sin(theta);

								yc++;
							}
						}
						xc++;
					}

					if (realVal > 0) bin_pat << 1;
					else bin_pat << 0;

					if (imagVal > 0) bin_pat << 1;
					else bin_pat << 0;

					//std::cout << realVal << "\t" << imagVal << "\n";
				}

				double result = 0.0;
				int pow = 1;
				for (k = bin_pat.str().length() - 1; k >= 0; --k, pow <<= 1)
					result += (bin_pat.str().at(k) - '0') * pow;

				result /= (255.0 / hist.rows);
				int res = (int)result;
				if (res == hist.rows) res--;
				hist.at<float>(res, 0) += 1.0;

				bin_pat.str("");
				bin_pat.clear();
			
		}
	}

	//cv::imshow("input", inputM);
	//cv::waitKey(1);

	//showHistogram(hist);

	return hist;
}

cv::Mat CSLBPHistogram(const cv::Mat& currentFrame)
{
	cv::Mat inputM = cv::Mat::zeros(currentFrame.size(), currentFrame.type());
	int i, j, k, l;
	

	cv::Mat hist = cv::Mat::zeros(16, 1, CV_32FC1);  ////16 bins	
	std::stringstream bin_pat;

	
	int upper_y = 0;
	int left_x = 0;
	int lower_y = currentFrame.rows;
	int right_x = currentFrame.cols;

	double thresh = 0.05; ////threshold for flat regions

	for (i = upper_y+2; i<lower_y-2; i++)
	{
		for (j = left_x+2; j<right_x-2; j++)
		{
				inputM.at<float>(i, j) = currentFrame.at<float>(i, j);
				for (k = (i - 1); k<(i + 2); k++)
				{
					for (l = (j - 1); l<(j + 2); l++)
					{
						if (k == i && l == j) continue;
						else
						{
							double diff = abs(currentFrame.at<float>(i, j) - currentFrame.at<float>(k, l));
							//std::cout << currentFrame.at<float>(i,j) << "\t" << currentFrame.at<float>(k,l) << "\t" << diff << "\n";

							if (diff > thresh)
								bin_pat << 1;
							else bin_pat << 0;
						}
					}
				}

				//binCounter++;
				double result = 0.0;
				int pow = 1;
				for (k = bin_pat.str().length() - 1; k >= 0; --k, pow <<= 1)
					result += (bin_pat.str().at(k) - '0') * pow;

				result /= (255.0 / hist.rows);
				int res = (int)result;
				if (res == hist.rows) res--;
				hist.at<float>(res, 0) += 1.0;

				bin_pat.str("");
				bin_pat.clear();
			
		}
	}

	//cv::imshow("input", inputM);
	//cv::waitKey(1);

	//showHistogram(hist);

	return hist;
}

void showHistogram(const cv::Mat& histogram)
{
	stringstream histrogramNameSS;
	histrogramNameSS << EnumStrings[featureTypeNum] << " Histogram";
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

void addTrainingDataAndLabels(string trainingDataPath, float label, vector<float> &labelsV, vector<Mat> &trainingHistogramsV)
{
	HANDLE hFind;
	WIN32_FIND_DATA data;
	wstring wideStr = string_to_wstring(trainingDataPath + "*.jpg");
	vector<string> filenamesV;
	hFind = FindFirstFile(wideStr.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		int cnt = 1;
		do {
			size_t size = wcslen(data.cFileName);
			char * buffer = new char[2 * size + 2];
			wcstombs(buffer, data.cFileName, 2 * size + 2);
			std::string file(buffer);
			delete[] buffer;

			cout << cnt << "  " << file << endl;
			filenamesV.push_back(file);

			cnt++;
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	Mat trainingImage, featureHistogramMat;
	string fileNumber, fileExtension;
	stringstream fileFull;
	fileExtension = ".jpg";



	for (int i = 0; i < filenamesV.size(); i++)
	{
		fileFull << trainingDataPath << filenamesV[i];
		trainingImage = imread(fileFull.str(), CV_LOAD_IMAGE_GRAYSCALE);
		trainingImage.convertTo(trainingImage, CV_32FC1);
		trainingImage /= 255;

		//for (int r = 0; r < trainingImage.rows; r++)
		//{
		//	for (int c = 0; c < trainingImage.cols; c++)
		//	{
		//		cout << trainingImage.at<float>(r, c) << "  ";
		//	}
		//	cout << endl;
		//}


		switch (featureTypeNum)
		{
		case LBP:
			featureHistogramMat = LBPHistogram(trainingImage);
			break;
		case LPQ:
			featureHistogramMat = LPQHistogram(trainingImage);
			break;
		case HOG:
			featureHistogramMat = HOGHistogram(trainingImage);
			break;
		case CSLBP:
			featureHistogramMat = CSLBPHistogram(trainingImage);
			break;
		default:
			break;
		}

		fileFull.str("");
		fileFull.clear();
		labelsV.push_back(label);
		trainingHistogramsV.push_back(featureHistogramMat);
	}
}

void trainSVMClassifier(string trainedSVMfilename, string positiveFilePath, string negativeFilePath, string identifier)
{

	vector<float> labelsV; //We store the labels of the training data here
	vector<Mat> trainingHistogramsV;
	addTrainingDataAndLabels(positiveFilePath, 1.0, labelsV, trainingHistogramsV);
	addTrainingDataAndLabels(negativeFilePath, -1.0, labelsV, trainingHistogramsV);

	//Create compare histogram distance marix
	Mat histogramDistMatrix = Mat::zeros(trainingHistogramsV.size(), trainingHistogramsV.size(), CV_32FC1);
	double value;
	for (int i = 0; i < trainingHistogramsV.size(); i++)
	{
		for (int j = 0; j < trainingHistogramsV.size(); j++)
		{
			value = compareHist(trainingHistogramsV[i], trainingHistogramsV[j], CV_COMP_BHATTACHARYYA);
			if (value > 1) value = 1;
			//cout << value << "   ";
			histogramDistMatrix.at<float>(i, j) = value;

		}
		//cout << endl << endl;
	}

	imshow("dist Mat", histogramDistMatrix);
	histogramDistMatrix *= 255;
	histogramDistMatrix.convertTo(histogramDistMatrix, CV_8UC1);
	stringstream histogramDistanceMatrixFilename;
	//histogramDistanceMatrixFilename << "D:/WorkFolder/LaneDetectionTrainingData/HistogramDistanceMatrix_" << EnumStrings[featureTypeNum] << ".jpg";
	//histogramDistanceMatrixFilename << "D:/PDP/SVMTrainingDataTool/HistogramDistanceMatrix_" << EnumStrings[featureTypeNum] << ".jpg";
	histogramDistanceMatrixFilename << histogramFilePath(EnumStrings[featureTypeNum], identifier);
	imwrite(histogramDistanceMatrixFilename.str(), histogramDistMatrix);
	waitKey(0);

	// Set up training data
	Mat labelsMat(labelsV.size(), 1, CV_32SC1);
	for (int i = 0; i < labelsMat.rows; i++)
	{
		labelsMat.at<int>(i, 0) = labelsV[i];
	}

	Mat trainingDataMat(labelsV.size(), trainingHistogramsV[0].rows, CV_32FC1); //The training matrix has every histogram loaded as a row
	for (int r = 0; r < trainingDataMat.rows; r++)
	{
		for (int c = 0; c < trainingDataMat.cols; c++) //0-31
		{
			trainingDataMat.at<float>(r, c) = trainingHistogramsV[r].at<float>(c, 0);
		}
	}

	//for (int r = 0; r < trainingDataMat.rows; r++)
	//{
	//	for (int c = 0; c < trainingDataMat.cols; c++)
	//	{
	//		cout << trainingDataMat.at<float>(r, c) << "  ";
	//	}
	//	cout << endl;
	//}


	//SVM is an abstract class now (OpenCV 3.0+) so we call SVM::create()
	cv::Ptr<SVM> svm = SVM::create();

	// Set up SVM's parameters
	svm->setType(SVM::C_SVC);
	//svm->setKernel(SVM::POLY);
	//svm->setDegree(1.0);
	//svm->setGamma(3.0);

	svm->setKernel(SVM::LINEAR); //There is a bug with OpenCV, can't save and load a SVM with polynomial kernel. Not sure about other types yet


								 //svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 0.01));

	// Train the SVM
	Ptr<ml::TrainData> tData = ml::TrainData::create(trainingDataMat, ml::SampleTypes::ROW_SAMPLE, labelsMat);
	svm->train(tData);
	svm->save(trainedSVMfilename);

	//Verify the SVM classifier 
	//		vector<double> responseV;
	//		Mat transposedMat;
	//		for (int i = 0; i < trainingHistogramsV.size(); i++)
	//		{
	//			transpose(trainingHistogramsV[i], transposedMat);
	//			responseV.push_back(svm->predict(transposedMat));
	//			
	////			cout << responseV[i] << endl;
	//		}

	//Output the result to a file
	//ofstream outputFile;
	//outputFile.open("D:/WorkFolder/LaneDetectionTrainingData/TrainingDataResponses.csv");
	//string expected;
	//for (int i = 0; i < responseV.size(); i++)
	//{
	//	if (labelsV[i]==1.0)
	//		expected = "Lanes";
	//	else
	//		expected = "NonLanes";
	//	outputFile << expected << "," << responseV[i] << endl;
	//}


	//Find accuracy of results on training data
	//int TP = 0;
	//int TN = 0;
	//int FP = 0;
	//int FN = 0;
	//for (int i = 0; i < responseV.size(); i++)
	//{
	//	if (labelsV[i] == responseV[i])
	//	{
	//		TP++;
	//	}
	//	else
	//	{
	//		FN++;
	//	}

	//}
	//float accuracy = (float)(TP + TN) / (TP + TN + FP + FN);
	//cout << "Accuracy: " << accuracy << endl;
	//outputFile << endl;
	//outputFile << "TP" << "," << TP << endl;
	//outputFile << "FN" << "," << FN << endl;
	//outputFile << "Accuracy" << "," << accuracy << endl;

}

std::vector<cv::Point> getSVMPrediction(int horizontalStart, int horizontalEnd, Mat &resizedImage, Mat &outputMat, cv::Ptr<SVM> &svm)
{
	Mat sampleBox = Mat(BOX_WIDTH, BOX_HEIGHT, CV_32FC1);
	Mat histogramOfFeature, transposedMat; //TODO: return the histogram transposed from the function
	int responseSVM = 0;
	std::vector<cv::Point> unfilteredLanePoints;
	cv::Point centerOfLaneBox;
	int cornerOffset = ceil((float)(VERTICAL_REGION_LOWER-VERTICAL_REGION_UPPER)/BOX_HEIGHT);
	int leftCornerOffset, rightCornerOffset;
	if (horizontalEnd == HORIZONTAL_CENTER) //We want to cut the upper left corner
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

				switch (featureTypeNum)
				{
				case LBP:
					histogramOfFeature = LBPHistogram(sampleBox);
					break;
				case LPQ:
					histogramOfFeature = LPQHistogram(sampleBox);
					break;
				case HOG:
					//histogramOfFeature = HOGHistogram(sampleBox);
					histogramOfFeature = HOGHistogramWithTranspose(sampleBox);
					break;
				case CSLBP:
					histogramOfFeature = CSLBPHistogram(sampleBox);
					break;
				default:
					break;
				}
				//cv::transpose(histogramOfFeature, transposedMat);
				//responseSVM = svm->predict(transposedMat);
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

void plotLanePoints(Mat &resizedImage, vector<Point> &leftLaneUnfilteredPoints, vector<Point> &rightLaneUnfilteredPoints)
{
	for (int i = 0; i < leftLaneUnfilteredPoints.size(); i++)
	{
		circle(resizedImage, leftLaneUnfilteredPoints[i], 3, Scalar(0, 0, 1.0), 2, 8, 0);
	}

	for (int i = 0; i < rightLaneUnfilteredPoints.size(); i++)
	{
		circle(resizedImage, rightLaneUnfilteredPoints[i], 3, Scalar(1.0, 0, 0), 2, 8, 0);
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
	vector<float> distancesToLineV;
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

float distanceToLine(cv::Point line_start, cv::Point line_end, cv::Point point)
{
	float normalLength = _hypot(line_end.x - line_start.x, line_end.y - line_start.y);
	float distance = (double)((point.x - line_start.x) * (line_end.y - line_start.y) - (point.y - line_start.y) * (line_end.x - line_start.x)) / normalLength;
	return abs(distance);
}

float euclideanDist(Point& p, Point& q)
{
	Point diff = p - q;
	return cv::sqrt((float)(diff.x*diff.x + diff.y*diff.y));
}

cv::Point findCentroidOfLaneArea(int rowStart, int columnStart, cv::Mat &sampleBox)
{
	sampleBox *= 255;
	sampleBox.convertTo(sampleBox, CV_8UC1);
		
	double otsu = 100;
	cv::Mat otsuMask(sampleBox.size(), sampleBox.type());
	threshold(sampleBox, otsuMask, otsu, 255, THRESH_OTSU);
	//adaptiveThreshold(sampleBox, otsuMask, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 15, 5);

	//Find contours of lane
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contoursV;
	cv::findContours(otsuMask, contoursV, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//Find largest contour
	int largestContourSize=0;
	int largestContourIndex=0;
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
	Moments mu = moments(contoursV[largestContourIndex], false);
	return Point((int)(mu.m10 / mu.m00) + columnStart, (int)(mu.m01 / mu.m00) + rowStart);
}