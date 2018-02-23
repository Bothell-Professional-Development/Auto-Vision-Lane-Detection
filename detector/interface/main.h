#pragma once

#include "ConfigFile.h"
#include "proj.h"

class __declspec(dllexport) CExampleExport
{
	bool running = false;
	double steerAngle = 0.0;
	double OutputArray[steerCommandArraySize];
	common_lib::ConfigFile cfgFile;
	ObjectEvent<InputContainer> detection_input;
	ObjectEvent<OutputContainer> detection_output;
	std::thread processingThread;

	InputContainer input;
	OutputContainer output;

	cv::Mat nframe;

public:
	unsigned int Initialize();
	double* DetectLanes(unsigned char* bufferCopy, const unsigned int bufferHeight, const unsigned int bufferWidth);
	unsigned int exit();
};
