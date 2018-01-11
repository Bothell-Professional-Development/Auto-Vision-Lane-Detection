// SVMTest.cpp : Defines the entry point for the console application.
//

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include "ConfigFile.h"
#include "PidController.h"
#include "generated.h"
#include "Detector.h"
#include "proj.h"

#include <iostream> //for make_pair
#include <fstream> //for ofstream
#include <utility>
#include <functional>
#include <ctime>

#include <csignal>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cmath>

#ifdef _WIN32
#include <io.h> //Check if file exists
#define access    _access_s
#else
#include <unistd.h>
#endif

using namespace cv;
using namespace cv::ml;
using namespace std;

class Fps30Video
{
public:
	Fps30Video(cv::VideoCapture &capture) :
		m_capture(&capture),
		m_time (clock())
	{}

	~Fps30Video()
	{
		if (m_capture)
		{
			m_capture = nullptr;
		}
	}

	bool read(cv::OutputArray &image)
	{
		using namespace std::chrono_literals;

		clock_t newTime = clock();
		bool read = false;
		double diff = difftime(newTime, m_time);
		if (diff > 0.040)
		{
			m_capture->read(image);
			read = true;
			m_time = clock();
		}

		return read;
	}

private:
	cv::VideoCapture *m_capture;
	clock_t m_time;
};

static const int bufferSize = 10;

double StdDev(int* new_value, double average, int size)
{
	int sum = 0;
	for (int i = 0; i < size; ++i)
	{
		sum += (new_value[i] - average) * (new_value[i] - average);
	}

	return std::sqrt(sum / size);
}
int fpsFilteredMean(int new_value)
{
	static int buffer[bufferSize] = { 0 };
	static unsigned int idx = 0;
	static double lowerBound = 0;
	static double average = 0;
	static double upperBound = 0;

	if (++idx <= bufferSize)
	{
		double scaling = 1. / (double)idx;

		buffer[idx - 1] = new_value;
		average = (new_value * scaling) + (average * (1 - scaling));

		//std::cout << "                                                     Average: " << average << "\n";

		if (idx == bufferSize)
		{
			double stdDev = StdDev(buffer, average, bufferSize);
			lowerBound = average - (stdDev * 2);
			upperBound = average + (stdDev * 2);

			//std::cout << "                                                     lowerBound: " << lowerBound << "\n";
			//std::cout << "                                                     upperBound: " << upperBound << "\n";

			for (int *f = (buffer + bufferSize - 1); f >= buffer; --f)
			{
				if (((*f) > upperBound) || ((*f) < lowerBound))
				{
					std::cout << "                                                     Bad:  " << (*f) << " (" << lowerBound << " < " << upperBound << ")\n";

					//if (f == (buffer + bufferSize - 1))
					//{
					//	std::cout << "                                                     Outted: ";
					//}

					//std::cout << (*f);
					(*f) = 0;
					--idx;
				}
				//else
				//{
				//	std::cout << "                                                     Good: " << (*f) << " (" << lowerBound << " < " << upperBound << ")\n";
				//}
			}

			if (idx < bufferSize)
			{
				//std::cout << "\n";

				average = 0;
				for (int i = 0; i < idx; ++i)
				{
					average += buffer[i];
				}

				average /= idx;
			}

			//std::cout << "                                                     "; 
			//for (int i = 0; i < bufferSize; ++i)
			//{
			//	std::cout << buffer[i] << " ";
			//}
			//std::cout << "\n";
		}
	}

	else if (lowerBound < new_value && upperBound > new_value)
	{
		buffer[(idx - 1) % bufferSize] = new_value;

		average = (new_value * 0.1) + (average * 0.9);

		double stdDev = StdDev(buffer, average, bufferSize);

		if (stdDev * 4 > average * 0.4)
		{
			lowerBound = average - (stdDev * 4);
			upperBound = average + (stdDev * 4);
		}
		else
		{
			lowerBound = average - (average * 0.4);
			upperBound = average + (average * 0.4);
		}

		//std::cout << "                                                     avg (" << average << ") low (" << lowerBound << ") high (" << upperBound << ")" << "   ";
		//for (int i = 0; i < bufferSize; ++i)
		//{
		//	std::cout << buffer[i] << " ";
		//}
		//std::cout << "\n";
	}

	return average;
}

int main()
{
	std::signal(SIGINT, KillHandler);

	clock_t prog_start_time = clock();
	clock_t prog_end_time;

	common_lib::ConfigFile cfgFile;
	cfgFile.pullValuesFromFile(CFG_FILE_PATH);

	//Load video
	cv::VideoCapture cap;
	cap.open(cfgFile.readValueOrDefault("DETECTOR_INPUT", ""));
	if (!cap.isOpened()) // Check for invalid input
	{
		std::cout << "Could not open or find the video file" << std::endl;
		return -1;
	}

	//Create and load already trained SVM classifier
	//Check if file exists
	
	if (access(cfgFile.readValueOrDefault("SVM_LEFT_MODEL", "").c_str(), 0) != 0)
	{
		std::cout << "Left SVM file doesn't exist" << std::endl;
		exit(-1);
	}
	if (access(cfgFile.readValueOrDefault("SVM_RIGHT_MODEL", "").c_str(), 0) != 0)
	{
		std::cout << "Right SVM file doesn't exist" << std::endl;
		exit(-1);
	}

	ObjectEvent<InputContainer> detection_input;
	ObjectEvent<OutputContainer> detection_output;
	std::thread processingThread(FrameProcessor, std::ref(cfgFile), std::ref(detection_input), std::ref(detection_output));
	Fps30Video video{ cap };

	InputContainer input;
	OutputContainer output;
	float fps;
	int fpsMean = 0;
	int droppedCycles = 0, caughtCycles = 0;
	int curveProgress = 0;
	double steerAngle = 0.0;

	while (gRunning)
	{
		//if (cap.read(input.frame))// && input.frame.data != NULL)
		if (video.read(input.frame))
		{
			//if (input.frame.data == NULL)
			if (input.frame.empty())
			{
				break;
			}

			if (detection_input.SetAndSignal(input))
			{
				caughtCycles++;
			}
			else
			{
				droppedCycles++;
			}
		}
		else
		{
			droppedCycles++;
		}

		//using namespace std::chrono_literals;
		//std::this_thread::sleep_for(10ms);

		if (detection_output.TryGetReset(output))
		{
			fps = 1000 / output.timePF;
			std::cout << "Processing time/frame " << output.frameCounter << " : " << output.timePF << " (" << fps << "fps)" << std::endl;
			// fpsMean = fpsFilteredMean(fps);
			fpsMean += fps;
#if WIN32
			if (output.outputMat.size.p && *(output.outputMat.size.p))
			{
				imshow("Classification", output.outputMat);
				waitKey(1);
			}
#endif
			curveProgress = 0;
		}

		steerAngle = set_steering_module(steerAngle);
		steerAngle = bezier_calc(curveProgress / 100, steerAngle, output.BezPointZero, output.BezPointOne, output.BezPointTwo, output.BezPointThree);
		curveProgress = (curveProgress + 1) % 100;
		std::cout << "Steer Angle " << steerAngle << " ( at " << curveProgress << " )" << std::endl;
	}

	//gRunning = false;
	std::raise(SIGINT);

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(10ms);

	if (output.frameCounter > FRAME_SKIP)
	{
		fpsMean = (int)fpsMean / (output.frameCounter / FRAME_SKIP);
	}

	std::cout << "Average fps rate: " << fpsMean << std::endl;
	std::cout << "Dropped / Caught: " << droppedCycles << "/" << caughtCycles << std::endl;

	prog_end_time = clock();
	std::cout << "Runtime : " << difftime(prog_end_time, prog_start_time) / (double)(CLOCKS_PER_SEC) << "\n";

	processingThread.join();
	//controlThread.join();

	std::cout << "Runtime : " << difftime(prog_end_time, prog_start_time) << "\n";
	std::cout << "Total points found: " << output.totalPointsFound << std::endl;
	
	return 0;
}
