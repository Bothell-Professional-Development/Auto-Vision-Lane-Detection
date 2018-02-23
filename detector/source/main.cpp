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

#ifdef LIBBuild
#include "main.h"
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
		if (diff > 40)
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

static bool* gRunning;
static void KillHandler(int signal)
{
	*gRunning = false;
}

#ifdef LIBBuild
unsigned int CExampleExport::Initialize()
{
	running = true;

	gRunning = &running;
	cfgFile.pullValuesFromFile(CFG_FILE_PATH);
	if (access(cfgFile.readValueOrDefault("SVM_LEFT_MODEL", "").c_str(), 0) != 0) { std::cout << "Left SVM file doesn't exist" << std::endl; exit(); }
	if (access(cfgFile.readValueOrDefault("SVM_RIGHT_MODEL", "").c_str(), 0) != 0) { std::cout << "Right SVM file doesn't exist" << std::endl; exit(); }
	
	processingThread = std::thread(FrameProcessor, std::ref(cfgFile), std::ref(detection_input), std::ref(detection_output), std::ref(running));
	return 0;
}

double* CExampleExport::DetectLanes(unsigned char* bufferCopy, const unsigned int bufferWidth, const unsigned int bufferHeight)
{
	nframe.create(bufferHeight, bufferWidth, CV_32FC3);
	std::memcpy(nframe.data, bufferCopy, nframe.elemSize());
	input.frame = nframe;

	if (input.frame.empty()) {
		return nullptr;
	}

	detection_input.SetAndSignal(input);
	detection_output.TryGetReset(output);

	steerAngle = bezier_calc(steerAngle, output.BezPointZero, output.BezPointOne, output.BezPointTwo, output.BezPointThree, OutputArray);
	return OutputArray;
}

unsigned int CExampleExport::exit()
{
	running = false;
	if (processingThread.joinable())
	{
		detection_input.SetAndSignal(input);
		processingThread.join();
	}

	return 0;
}
#else
int main()
{
	bool running = true;
	gRunning = &running;

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
	std::thread processingThread(FrameProcessor, std::ref(cfgFile), std::ref(detection_input), std::ref(detection_output), std::ref(running));
	Fps30Video video{ cap };

	InputContainer input;
	OutputContainer output;
	float fps;
	int fpsMean = 0;
	int droppedCycles = 0, caughtCycles = 0;
	int curveProgress = 0;
	const double curveProgressMax = 30;
	double steerAngle = 0.0;
	float steerAngleAvg = 0.0;
	int avgSteps = 0;
	double OutputArray[steerCommandArraySize] = { 0. };

	while (running)
	{
		if (video.read(input.frame))
		{
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

		if (detection_output.TryGetReset(output))
		{
			fps = 1000 / output.timePF;
			std::cout << "\nProcessing time/frame " << output.frameCounter << " : " << output.timePF << " (" << fps << "fps)" << 
				" :: last angle: " << (steerAngle) << " progress: " << curveProgress << " steps" << " ( ";
			for (int i = 0; i < steerCommandArraySize; ++i) { std::cout << std::fixed << OutputArray[i] << " "; }
			std::cout << ")" << "\n";

			fpsMean += fps;
#if WIN32
			if (output.outputMat.size.p && *(output.outputMat.size.p))
			{
				imshow("Classification", output.outputMat);
				waitKey(1);
			}
#endif
			avgSteps += curveProgress;
			curveProgress = 0;
			steerAngleAvg = 0;
		}

		steerAngle = set_steering_module(steerAngle);
		steerAngle = bezier_calc(steerAngle, output.BezPointZero, output.BezPointOne, output.BezPointTwo, output.BezPointThree, OutputArray);
		steerAngleAvg += (float)steerAngle;
	}

	std::raise(SIGINT);

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(10ms);

	if (output.frameCounter > FRAME_SKIP)
	{
		fpsMean = (int)fpsMean / (output.frameCounter / FRAME_SKIP);
	}

	std::cout << "Average fps rate       : " << fpsMean << std::endl;
	std::cout << "Avg Curve Progress     : " << avgSteps / (output.frameCounter / FRAME_SKIP) << std::endl;
	std::cout << "Dropped / Caught       : " << droppedCycles << "/" << caughtCycles << std::endl;

	prog_end_time = clock();
	std::cout << "Runtime                : " << difftime(prog_end_time, prog_start_time) / (double)(CLOCKS_PER_SEC) << "\n";
	std::cout << "Total points found     : " << output.totalPointsFound << std::endl;

	processingThread.join();
	return 0;
}
#endif
