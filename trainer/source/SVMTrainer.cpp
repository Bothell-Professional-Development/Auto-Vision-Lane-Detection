//Receives as input 3 folders with 30x30 images divided into 3 classes: Left lane, Left non-lane, Right lane, and Right non-lane
//Produces as output 2 xml files containing the SVM classifiers for left lane/non-lane and right lane/non-lane classification
#define UNICODE

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
#include <direct.h> //_getcwd
#include <atlbase.h> //For LPCWSTR and CreateFolder
#include <ctime>
#include <stdio.h>

using namespace cv;
using namespace cv::ml;
using namespace std;

cv::Mat HOGHistogram(const cv::Mat& currentFrame);
void showHistogram(const cv::Mat& histogram);
void addTrainingDataAndLabels(string trainingDataPath, float label, vector<float> &labelsV, vector<Mat> &trainingHistogramsV);
void trainSVMClassifier(string trainedSVMfilename, string positiveFilePath, string negativeFilePath, string folderPath, string identifier);

//const string positiveRightFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/ChristosDataset/positiveRight/";
//const string positiveLeftFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/ChristosDataset/positiveLeft/";
//const string negativeLeftFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/ChristosDataset/negative/";
//const string negativeRightFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/ChristosDataset/negative/";

const string positiveRightFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/NewDataSet/positive/right/";
const string positiveLeftFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/NewDataSet/positive/left/";
const string negativeLeftFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/NewDataSet/negative/right/";
const string negativeRightFilePath = "D:/WorkFolder/LaneDetectionTrainingData/TrainingDataSets/NewDataSet/negative/left/";

#define trainingFilePath(currentPath, featureString, laneSide) currentPath << "/SVM_" << featureString << "_" << laneSide << "Lane.xml"
#define histogramFilePath(currentPath, featureString, identifier) currentPath << "/HistogramDistanceMatrix_" << featureString << "_" << identifier << ".jpg";


std::wstring string_to_wstring(const std::string& text) {
	return std::wstring(text.begin(), text.end());
}


int main()
{
	char cCurrentPath[200];
	_getcwd(cCurrentPath, sizeof(cCurrentPath));	
	stringstream outputFolder;
	outputFolder << cCurrentPath << "/../SVMOutput";
	USES_CONVERSION;
	CreateDirectory(A2W(outputFolder.str().c_str()), NULL);

	stringstream trainedSVMLeftLanefilename;
	stringstream trainedSVMRightLanefilename;
	trainedSVMLeftLanefilename << trainingFilePath(outputFolder.str(), "HOG", "Left");
	trainSVMClassifier(trainedSVMLeftLanefilename.str(), positiveLeftFilePath, negativeLeftFilePath, outputFolder.str(), "Left");
	trainedSVMRightLanefilename << trainingFilePath(outputFolder.str(), "HOG", "Right");
	trainSVMClassifier(trainedSVMRightLanefilename.str(), positiveRightFilePath, negativeRightFilePath, outputFolder.str(), "Right");
	return 0;
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


void showHistogram(const cv::Mat& histogram)
{
	stringstream histrogramNameSS;
	histrogramNameSS << "HOG Histogram";
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

		featureHistogramMat = HOGHistogram(trainingImage);

		fileFull.str("");
		fileFull.clear();
		labelsV.push_back(label);
		trainingHistogramsV.push_back(featureHistogramMat);
	}
}

void trainSVMClassifier(string trainedSVMfilename, string positiveFilePath, string negativeFilePath, string folderPath, string identifier)
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
	}

	imshow("dist Mat", histogramDistMatrix);
	histogramDistMatrix *= 255;
	histogramDistMatrix.convertTo(histogramDistMatrix, CV_8UC1);
	stringstream histogramDistanceMatrixFilename;
	histogramDistanceMatrixFilename << histogramFilePath(folderPath, "HOG", identifier);
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

	//SVM is an abstract class now (OpenCV 3.0+) so we call SVM::create()
	cv::Ptr<SVM> svm = SVM::create();

	// Set up SVM's parameters
	svm->setType(SVM::C_SVC);
	//svm->setKernel(SVM::POLY);
	//svm->setDegree(1.0);
	//svm->setGamma(3.0);

	svm->setKernel(SVM::LINEAR); //There is a bug with OpenCV, can't save and load a SVM with polynomial kernel. Not sure about other types yet
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 0.01));

	// Train the SVM
	Ptr<ml::TrainData> tData = ml::TrainData::create(trainingDataMat, ml::SampleTypes::ROW_SAMPLE, labelsMat);
	svm->train(tData);
	svm->save(trainedSVMfilename);
}

