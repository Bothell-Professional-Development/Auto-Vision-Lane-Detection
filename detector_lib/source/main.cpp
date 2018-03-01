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

#include "main.h"

using namespace cv;
using namespace cv::ml;
using namespace std;

static bool* gRunning;
static void KillHandler(int signal)
{
	*gRunning = false;
}

unsigned int CExampleExport::Initialize(const bool simulatorInput,
										const float valMax,
                                        const float valMin,
                                        const float kp,
                                        const float kd,
                                        const float ki)
{
	running = true;
	m_simulatorInput = simulatorInput;

	gRunning = &running;
	cfgFile.pullValuesFromFile(CFG_FILE_PATH);
	if (access(cfgFile.readValueOrDefault("SVM_LEFT_MODEL", "").c_str(), 0) != 0) { std::cout << "Left SVM file doesn't exist" << std::endl; exit(); }
	if (access(cfgFile.readValueOrDefault("SVM_RIGHT_MODEL", "").c_str(), 0) != 0) { std::cout << "Right SVM file doesn't exist" << std::endl; exit(); }
	
	if (simulatorInput)
	{
		pid = new PIDController(valMax, valMin, kp, kd, ki);
	}
	
	processingThread = std::thread(FrameProcessor, std::ref(cfgFile), std::ref(detection_input), std::ref(detection_output), std::ref(running));
	
	return 0;
}

double* CExampleExport::DetectLanes(unsigned char* bufferCopy, const unsigned int bufferHeight, const unsigned int bufferWidth, const float dt)
{
	if (m_simulatorInput)
	{
		nframe = cv::Mat(bufferHeight, bufferWidth, CV_8UC4, bufferCopy);
		// Mat testRGB;
		cv::cvtColor(nframe, nframe, cv::COLOR_BGRA2RGB);
	}
	else
	{
		nframe = cv::Mat(bufferHeight, bufferWidth, CV_8UC3, bufferCopy);
	}

	input.frame = nframe;
    
	if (input.frame.empty()) {
		return nullptr;
	}

	detection_input.SetAndSignal(input);
	detection_output.TryGetReset(output);

	if (output.outputMat.size.p && *(output.outputMat.size.p))
	{
		imshow("Classification", output.outputMat);
		waitKey(1);
		if (m_simulatorInput)
		{
			steerAngle = pid->Calculate(400.0f, output.BezPointThree.x, dt);
		}
		else
		{
			steerAngle = bezier_calc(steerAngle, output.BezPointZero, output.BezPointOne, output.BezPointTwo, output.BezPointThree, OutputArray);
		}
	}

	if (m_simulatorInput)
	{
		return &steerAngle;
	}
	else
	{
		return OutputArray;
	}
}

unsigned int CExampleExport::exit()
{
	running = false;
	
	if (pid)
	{
		delete pid;
	}

	if (processingThread.joinable())
	{
		detection_input.SetAndSignal(input);
		processingThread.join();
	}

	return 0;
}
