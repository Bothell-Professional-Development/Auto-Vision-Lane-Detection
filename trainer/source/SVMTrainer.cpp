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

cv::Mat LBPHistogram(const cv::Mat& currentFrameSegment);
cv::Mat HOGHistogram(const cv::Mat& currentFrame);
cv::Mat HOGHistogramWithTranspose(const cv::Mat& currentFrame);
cv::Mat LPQHistogram(const cv::Mat& currentFrame);
cv::Mat CSLBPHistogram(const cv::Mat& currentFrame);
void showHistogram(const cv::Mat& histogram);
void addTrainingDataAndLabels(string trainingDataPath, float label, vector<float> &labelsV, vector<Mat> &trainingHistogramsV);
void trainSVMClassifier(string trainedSVMfilename, string positiveFilePath, string negativeFilePath, string folderPath, string identifier);

const string positiveRightFilePath = "D:/PDP/SVMTrainingDataTool/positive/right/";
const string positiveLeftFilePath = "D:/PDP/SVMTrainingDataTool/positive/left/";
const string negativeLeftFilePath = "D:/PDP/SVMTrainingDataTool/negative/left/";
const string negativeRightFilePath = "D:/PDP/SVMTrainingDataTool/negative/right/";

#define trainingFilePath(currentPath, featureString, laneSide) currentPath << "/SVM_" << featureString << "_" << laneSide << "Lane.xml"
#define histogramFilePath(currentPath, featureString, identifier) currentPath << "/HistogramDistanceMatrix_" << featureString << "_" << identifier << ".jpg";

typedef enum {
	LBP, // is 0
	LPQ, // is 1
	HOG, // is 2
	CSLBP
};
const int featureTypeNum = HOG;  //Choose feature type
static const char * EnumStrings[] = { "LBP", "LPQ", "HOG", "CSLBP" };



std::wstring string_to_wstring(const std::string& text) {
	return std::wstring(text.begin(), text.end());
}


int main()
{

	char cCurrentPath[200];
	_getcwd(cCurrentPath, sizeof(cCurrentPath));
	
	stringstream outputFolder;
	outputFolder << cCurrentPath << "/SVMOutput";
	USES_CONVERSION;
	CreateDirectory(A2W(outputFolder.str().c_str()), NULL);

	stringstream trainedSVMLeftLanefilename;
	stringstream trainedSVMRightLanefilename;
	trainedSVMLeftLanefilename << trainingFilePath(outputFolder.str(), EnumStrings[featureTypeNum], "Left");
	trainSVMClassifier(trainedSVMLeftLanefilename.str(), positiveLeftFilePath, negativeLeftFilePath, outputFolder.str(), "Left");

	trainedSVMRightLanefilename << trainingFilePath(outputFolder.str(), EnumStrings[featureTypeNum], "Right");
	trainSVMClassifier(trainedSVMRightLanefilename.str(), positiveRightFilePath, negativeRightFilePath, outputFolder.str(), "Right");

	return 0;
}



cv::Mat LBPHistogram(const cv::Mat& currentFrameSegment)
{
	int i, j, k, l;
	cv::Mat hist = cv::Mat::zeros(32, 1, CV_32FC1);  ////32 bins	
	std::stringstream bin_pat;

	//Find the upper left and lower right corner of the blob's bounding box

	for (int i = 1; i<currentFrameSegment.rows - 3; i++)
	{
		for (int j = 1; j<currentFrameSegment.cols - 3; j++)
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

	for (i = upper_y + 2; i<lower_y - 2; i++)
	{
		for (j = left_x + 2; j<right_x - 2; j++)
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
		//cout << endl << endl;
	}

	imshow("dist Mat", histogramDistMatrix);
	histogramDistMatrix *= 255;
	histogramDistMatrix.convertTo(histogramDistMatrix, CV_8UC1);
	stringstream histogramDistanceMatrixFilename;
	histogramDistanceMatrixFilename << histogramFilePath(folderPath, EnumStrings[featureTypeNum], identifier);
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

