#pragma once
#include "afx.h"

#include "ConfigFile.h"
#include "proj.h"

class __declspec(dllexport) CExampleExport : public CObject
{
	bool running = false;
	double steerAngle = 0.0;
	common_lib::ConfigFile cfgFile;
	ObjectEvent<InputContainer> detection_input;
	ObjectEvent<OutputContainer> detection_output;
	std::thread processingThread;
public:
	unsigned int Initialize();
	float DetectLanes(unsigned char* bufferCopy, const unsigned int bufferWidth, const unsigned int bufferHeight);
	unsigned int exit();
};
