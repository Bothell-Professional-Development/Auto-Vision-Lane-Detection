#pragma once

#include "ConfigFile.h"
#include "proj.h"
#include "PidController.h"

class __declspec(dllexport) CExampleExport
{
	bool running = false;
	double OutputArray[steerCommandArraySize];
	common_lib::ConfigFile cfgFile;
	ObjectEvent<InputContainer> detection_input;
	ObjectEvent<OutputContainer> detection_output;
	std::thread processingThread;

	bool m_simulatorInput;

	InputContainer input;
	OutputContainer output;

	cv::Mat nframe;

    PIDController* pid;

public:
	unsigned int Initialize(const bool simulatorInput,
		                    const float valMax,
                            const float valMin,
                            const float kp,
                            const float kd,
                            const float ki);
	double* DetectLanes(unsigned char* bufferCopy, const unsigned int bufferHeight, const unsigned int bufferWidth, const float dt);
	unsigned int exit();
};
