#include "ImageProcessor.h"
#include <iostream>

ImageProcessor::ImageProcessor(double cannyLowThreshold,
                               double cannyHighThreshold,
                               int32_t cannyAperture,
                               double houghRho,
                               double houghTheta,
                               int32_t houghThreshold,
                               double houghMinLength,
                               double houghMaxGap,
							   int32_t roiUpperLimit,
							   int32_t roiLowerLimit) :
    m_cannyLowThreshold(cannyLowThreshold),
    m_cannyHighThreshold(cannyHighThreshold),
    m_cannyAperture(cannyAperture),
    m_houghRho(houghRho),
    m_houghTheta(houghTheta),
    m_houghThreshold(houghThreshold),
    m_houghMinLength(houghMinLength),
    m_houghMaxGap(houghMaxGap),
	m_roiUpperLimit(roiUpperLimit),
	m_roiLowerLimit(roiLowerLimit)
{    
	element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(2, 2));
}

cv::Mat ImageProcessor::LaneFilter(cv::Mat &inputMat)
{
    cv::Mat outputMat;
    cv::cvtColor(inputMat, outputMat, cv::COLOR_RGB2HLS);
	cv::Mat whiteMasked;
    cv::inRange(outputMat, cv::Scalar(0, 200, 0), cv::Scalar(255, 255, 255), whiteMasked);
    cv::Mat yellowMasked;
    cv::inRange(outputMat, cv::Scalar(70, 0, 100), cv::Scalar(110, 255, 255), yellowMasked);
    cv::Mat laneMasked;
	
    cv::bitwise_or(whiteMasked, yellowMasked, laneMasked);
	cv::bitwise_and(inputMat, inputMat, outputMat, laneMasked);
	
    return laneMasked;
}

cv::Mat ImageProcessor::GrayscaleImage(cv::Mat &inputMat)
{
    cv::Mat outputMat = cv::Mat(inputMat.rows, inputMat.cols, CV_8UC1);
    cv::cvtColor(inputMat, outputMat, cv::COLOR_RGB2GRAY);

    return outputMat;  
}

cv::Mat ImageProcessor::CannyEdgesDetector(cv::Mat &inputMat)
{
    cv::Canny(inputMat, inputMat, m_cannyLowThreshold, m_cannyHighThreshold, m_cannyAperture);
    //cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2), cv::Point(-1, -1));
    //cv::dilate(inputMat, inputMat, element);

    return inputMat;
}

std::vector<cv::Vec4i> ImageProcessor::HoughLinesDetector( cv::Mat &inputMat)
{            
    std::vector<cv::Vec4i> lines;

    HoughLinesP(inputMat, lines, m_houghRho, m_houghTheta, m_houghThreshold, m_houghMinLength, m_houghMaxGap);

    return lines;
}

cv::Scalar ImageProcessor::FindMedianColorOfLane(cv::Mat &inputMat, std::vector<cv::Vec4i> laneLine)
{
	cv::Scalar laneColor;
	cv::Mat mask = cv::Mat::zeros(inputMat.rows, inputMat.cols, CV_8UC1);
	
	for (int i = 0; i < laneLine.size(); i++)
	{
		cv::line(mask, cv::Point(laneLine[i][0], laneLine[i][1]), cv::Point(laneLine[i][2], laneLine[i][3]), 255, 1, 8, 0);
	}

	//Dilation of the lines will give us the broader area of the lanes
	cv::dilate(mask, mask, element);
	//Place the mask on the input image to get the color
	inputMat.copyTo(mask, mask);
	
	int nonZeroPixelCounter = 0;
	int redChannelMean = 0;
	int greenChannelMean = 0;
	int blueChannelMean = 0;
	//cv::imshow("mask test", mask);
	
	//for (int r = m_roiUpperLimit; r < m_roiLowerLimit; r++) //Find the mean color of the lane area within ROI
	for (int r=0; r<mask.rows; r++)
	{
		for (int c = 0; c < mask.cols; c++)
		{
			if (mask.at<cv::Vec3b>(r, c).val[0] != 0 || mask.at<cv::Vec3b>(r, c).val[1] != 0 || mask.at<cv::Vec3b>(r, c).val[2] != 0)
			{
				redChannelMean += mask.at<cv::Vec3b>(r, c).val[2];
				greenChannelMean += mask.at<cv::Vec3b>(r, c).val[1];
				blueChannelMean += mask.at<cv::Vec3b>(r, c).val[0];
				nonZeroPixelCounter++;
			}
		}
	}

	if (nonZeroPixelCounter != 0)
	{
		redChannelMean = (int)redChannelMean / nonZeroPixelCounter;
		greenChannelMean = (int)greenChannelMean / nonZeroPixelCounter;
		blueChannelMean = (int)blueChannelMean / nonZeroPixelCounter;
	}
	laneColor = cv::Scalar(blueChannelMean, greenChannelMean, redChannelMean, 0.0);
	//cv::imshow("Mask", mask);
	cv::Mat colorMatTest = cv::Mat(mask.size(), mask.type());
	colorMatTest = laneColor;
	//cv::imshow("colorMatTest", colorMatTest);

	return laneColor;
}