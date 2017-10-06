#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <ctime>

#include "ImageProcessor.h"
#include "LaneDetector.h"


/// Global Variables
const int voteSliderMax = 500;
int voteSlider = 50;

const double  CANNY_LOW_THRESHOLD = 100.0;
const double  CANNY_HIGH_THRESHOLD = 300.0;

const int32_t CANNY_APERTURE       = 3;
const double  HOUGH_RHO            = 1.0;
const double  HOUGH_THETA          = CV_PI / 180;
const int32_t HOUGH_THRESHOLD      = 50;
const double  HOUGH_MIN_LENGTH = 25.0;
const double  HOUGH_MAX_GAP = 5.0;

const int32_t  VERTICAL_REGION_UPPER = 120;
const int32_t  VERTICAL_REGION_LOWER = 190;
const int32_t  HORIZONTAL_REGION_LEFT = 50;
const int32_t  HORIZONTAL_REGION_RIGHT = 430;
const double  LANE_CONVERGENCE_ANGLE = 45.0;
const double  LANE_CONVERGENCE_ANGL_TOL = 20.0;

const int32_t HORIZONTAL_RESOLUTION = 480;
const int32_t VERTICAL_RESOLUTION = 270;

const int32_t IMSHOW_HORIZONTAL_OFFSET = 50;
const int32_t IMSHOW_VERTICAL_OFFSET = 100;
const int32_t IMSHOW_WINDOW_HEADER_OFFSET = 50;
const int32_t IMSHOW_WINDOW_SPACE = 20;

const bool IMSHOW_ON = false;

cv::Mat inputMat, outputMat;
cv::Scalar leftLaneColor = cv::Scalar(-1, -1, -1);
cv::Scalar rightLaneColor = cv::Scalar(-1, -1, -1);


int main(int argc, char** argv)
{
    cv::VideoCapture cap;
    //cap.open(argv[1]);
	cap.open("D:\\WorkFolder\\Auto-Vision-Lane-Detection-build\\vids\\test3.mp4");
	
	if (!cap.isOpened()) // Check for invalid input
	{
		std::cout << "Could not open or find the video file" << std::endl;
		return -1;
	}

    cv::Mat frame; 
    cv::Mat resizedImage;
    cv::Mat outputImage;
	cv::Mat leftOutputImage, rightOutputImage;
	cv::Mat roiMat;
	cv::Mat leftHalfImage, rightHalfImage;

	ImageProcessor imagePrc(CANNY_LOW_THRESHOLD,
                            CANNY_HIGH_THRESHOLD,
                            CANNY_APERTURE,
                            HOUGH_RHO,
                            HOUGH_THETA,
                            HOUGH_THRESHOLD,
                            HOUGH_MIN_LENGTH,
                            HOUGH_MAX_GAP,
							VERTICAL_REGION_UPPER,
							VERTICAL_REGION_LOWER);


    LaneDetector detector(VERTICAL_REGION_UPPER,
                          VERTICAL_REGION_LOWER,
						  HORIZONTAL_REGION_RIGHT - HORIZONTAL_REGION_LEFT, //HORIZONTAL_RESOLUTION,
                          LANE_CONVERGENCE_ANGLE,
                          LANE_CONVERGENCE_ANGL_TOL);

	int timePF = 1;
	float fps;
	clock_t oldTime= clock();
	clock_t newTime;
	int fpsMean=0;
	int frameCounter = 0;
    while (cap.read(frame) && frame.data != NULL)
    {   
		//Get the processing time per frame
		newTime = clock();
		timePF = newTime - oldTime;
		oldTime = newTime;
		fps = 1000 / timePF;
		std::cout << "Processing time/frame: " << timePF << " (" << fps << "fps)" << std::endl;
		fpsMean += fps;
		frameCounter++;

        resize(frame, resizedImage, cv::Size(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION));
        resizedImage.convertTo(resizedImage, CV_8UC3);

		//Create a new image using only the ROI
		roiMat = cv::Mat(resizedImage, cv::Range(VERTICAL_REGION_UPPER, VERTICAL_REGION_LOWER), cv::Range(HORIZONTAL_REGION_LEFT, HORIZONTAL_REGION_RIGHT));


		// TESTING: Mark the aRea Of Interest ROI
		cv::line(resizedImage, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER), cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(resizedImage, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(resizedImage, cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_LEFT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);
		cv::line(resizedImage, cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_UPPER), cv::Point(HORIZONTAL_REGION_RIGHT, VERTICAL_REGION_LOWER), cv::Scalar(0, 255, 0), 2, 8, 0);

		if (IMSHOW_ON)
		{
			cv::namedWindow("Input Video", cv::WINDOW_AUTOSIZE); // create a window for display.
			cv::moveWindow("Input Video", IMSHOW_HORIZONTAL_OFFSET, IMSHOW_VERTICAL_OFFSET);
			cv::imshow("Input Video", resizedImage); // show our image inside it.
		}
		imagePrc.ImageSplitter(leftHalfImage, rightHalfImage, roiMat); //Split image into 2 halves to detect lanes easier

		//outputImage = imagePrc.LaneFilter(roiMat); //leftLaneColor and rightLaneColor should be fed back to LaneFilter
		leftOutputImage = imagePrc.LaneFilter(leftHalfImage);
		rightOutputImage = imagePrc.LaneFilter(rightHalfImage);

		if (IMSHOW_ON)
		{
			cv::namedWindow("Left HSL", cv::WINDOW_AUTOSIZE);
			cv::moveWindow("Left HSL", IMSHOW_HORIZONTAL_OFFSET, IMSHOW_VERTICAL_OFFSET + resizedImage.rows + IMSHOW_WINDOW_HEADER_OFFSET);
			cv::imshow("Left HSL", leftOutputImage);
			cv::namedWindow("Right HSL", cv::WINDOW_AUTOSIZE);
			cv::moveWindow("Right HSL", IMSHOW_HORIZONTAL_OFFSET + leftOutputImage.cols, IMSHOW_VERTICAL_OFFSET + resizedImage.rows + IMSHOW_WINDOW_HEADER_OFFSET);
			cv::imshow("Right HSL", rightOutputImage);
			//outputImage = imagePrc.GrayscaleImage(resizedImage);
			//cv::namedWindow("B&W", cv::WINDOW_AUTOSIZE);
			//cv::moveWindow("B&W", IMSHOW_HORIZONTAL_OFFSET + (IMSHOW_WINDOW_SPACE + outputImage.cols), IMSHOW_VERTICAL_OFFSET + outputImage.rows + IMSHOW_WINDOW_HEADER_OFFSET);
			//cv::imshow("B&W", outputImage);
		}

		std::vector<cv::Vec4i> leftLines = imagePrc.HoughLinesDetector(leftOutputImage);
		std::vector<cv::Vec4i> rightLines = imagePrc.HoughLinesDetector(rightOutputImage);

		cv::Mat leftHoughMat = leftOutputImage.clone();
		cv::Mat rightHoughMat = rightOutputImage.clone();

		cv::cvtColor(leftHoughMat, leftHoughMat, cv::COLOR_GRAY2RGB);
		cv::cvtColor(rightHoughMat, rightHoughMat, cv::COLOR_GRAY2RGB);

		for (size_t i = 0; i < leftLines.size(); i++)
		{
			cv::Vec4i l = leftLines[i];
			cv::line(leftHoughMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, 8, 0);
		}
		
		for (size_t i = 0; i < rightLines.size(); i++)
		{
			cv::Vec4i l = rightLines[i];
			cv::line(rightHoughMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 1, 8, 0);
		}

		if (IMSHOW_ON)
		{
			cv::namedWindow("Left Hough Lines", cv::WINDOW_AUTOSIZE);
			cv::moveWindow("Left Hough Lines", IMSHOW_HORIZONTAL_OFFSET + (IMSHOW_WINDOW_SPACE + resizedImage.cols), IMSHOW_VERTICAL_OFFSET);
			cv::imshow("Left Hough Lines", leftHoughMat);
			cv::namedWindow("Right Hough Lines", cv::WINDOW_AUTOSIZE);
			cv::moveWindow("Right Hough Lines", IMSHOW_HORIZONTAL_OFFSET + (IMSHOW_WINDOW_SPACE + resizedImage.cols) + leftHoughMat.cols, IMSHOW_VERTICAL_OFFSET);
			cv::imshow("Right Hough Lines", rightHoughMat);
		}

		std::vector<cv::Vec4i> filteredLeftLines = detector.FilterHoughLines(leftLines);
		std::vector<cv::Vec4i> filteredRightLines = detector.FilterHoughLines(rightLines);
		
		cv::Mat leftFilteredMat(leftOutputImage.size(), CV_8UC1);
		cv::cvtColor(leftFilteredMat, leftFilteredMat, cv::COLOR_GRAY2RGB); //Might not be needed if mat defined as 3-channels
		cv::Mat rightFilteredMat(rightOutputImage.size(), CV_8UC1);
		cv::cvtColor(rightFilteredMat, rightFilteredMat, cv::COLOR_GRAY2RGB); //Might not be needed if mat defined as 3-channels

		for (size_t i = 0; i < filteredLeftLines.size(); i++)
		{
			cv::Vec4i l = filteredLeftLines[i];
			cv::line(leftFilteredMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 1, 8, 0); //Red lines for left lane
		}

		for (size_t i = 0; i < filteredRightLines.size(); i++)
		{
			cv::Vec4i l = filteredRightLines[i];
			cv::line(rightFilteredMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 1, 8, 0); //Blue lines for right lane
		}

		//Getting the (mean) color of each lane will help us detect the lane on the next frame
		//as we will narrow down the color range of our search
		//leftLaneColor = imagePrc.FindMedianColorOfLane(roiMat, filteredLeftLines);
		//rightLaneColor = imagePrc.FindMedianColorOfLane(roiMat, filteredRightLines);

		if (IMSHOW_ON)
		{
			cv::namedWindow("Left Filtered Lines", cv::WINDOW_AUTOSIZE);
			cv::moveWindow("Left Filtered Lines", IMSHOW_HORIZONTAL_OFFSET + 2 * (IMSHOW_WINDOW_SPACE + resizedImage.cols), IMSHOW_VERTICAL_OFFSET);
			cv::imshow("Left Filtered Lines", leftFilteredMat);
			cv::namedWindow("Right Filtered Lines", cv::WINDOW_AUTOSIZE);
			cv::moveWindow("Right Filtered Lines", IMSHOW_HORIZONTAL_OFFSET + 2 * (IMSHOW_WINDOW_SPACE + resizedImage.cols) + leftFilteredMat.cols, IMSHOW_VERTICAL_OFFSET);
			cv::imshow("Right Filtered Lines", rightFilteredMat);
			cv::waitKey(0);
		}
    }
	if (frameCounter != 0)
	{
		fpsMean = (int)fpsMean / frameCounter;
	}
	std::cout << "Average fps rate: " << fpsMean << std::endl;

    return 0;
}