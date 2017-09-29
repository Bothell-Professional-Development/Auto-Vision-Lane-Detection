#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class ImageProcessor
{

public:
	ImageProcessor(double cannyLowThreshold,
		           double cannyHighThreshold,
		           int32_t cannyAperture,
		           double houghRho,
				   double houghTheta,
				   int32_t houghThreshold,
				   double houghMinLength,
				   double houghMaxGap,
				   int32_t roiUpperLimit,
				   int32_t roiLowerLimit);

    cv::Mat LaneFilter(cv::Mat & inputMat);

	cv::Mat GrayscaleImage(cv::Mat &inputMat);
	cv::Mat CannyEdgesDetector(cv::Mat &inputMat);
    std::vector<cv::Vec4i> HoughLinesDetector(cv::Mat &inputMat);
	cv::Scalar FindMedianColorOfLane(cv::Mat &inputMat, std::vector<cv::Vec4i> laneLine);


private:
	cv::Mat element;
	double m_cannyLowThreshold;
	double m_cannyHighThreshold;
	int32_t m_cannyAperture;

	double m_houghRho;
	double m_houghTheta;
	int32_t m_houghThreshold;
	double m_houghMinLength;
	double m_houghMaxGap;
	int32_t m_roiUpperLimit;
	int32_t m_roiLowerLimit;
};
#endif