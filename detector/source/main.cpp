// SVMTest.cpp : Defines the entry point for the console application.
//
#ifdef _WIN32
#include <Windows.h>
#endif

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

int main(string msg)
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
	std::string file = cfgFile.readValueOrDefault("DETECTOR_INPUT", "");

	std::cout << "Video path " << file << "\n";

	if (file.size() == 1 && isdigit(file.front()))
	{
		// For opening web camera
		std::string::size_type sz;
		int i_camera = std::stoi(file, &sz);
		cap.open(i_camera);
	}
	else
	{
		// Open a video file on disk
		cap.open(file);
	}

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
				" :: last angle: " << (output.SteerAngle) << " progress: " << curveProgress << " steps" << " ( ";
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

		output.SteerAngle = set_steering_module(output.SteerAngle);
		output.SteerAngle = bezier_calc(output.SteerAngle, output.BezPointZero, output.BezPointOne, output.BezPointTwo, output.BezPointThree, OutputArray);
		steerAngleAvg += (float)output.SteerAngle;
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

#ifdef _WIN32

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define ID_BUTTON_GRID             1
#define ID_BUTTON_CIRCLE           2
#define ID_BUTTON_ROI              3
#define ID_BUTTON_MARGIN_LINES     4
#define ID_BUTTON_BEZIER           5
#define ID_BUTTON_DETECTED_LANE    6

HWND hndl_Grid = NULL;
HWND hndl_Circle = NULL;
HWND hndl_ROI = NULL;
HWND hndl_MarginLines = NULL;
HWND hndl_Bezier = NULL;
HWND hndl_DetectedLane = NULL;


LPWSTR getButtonString(int buttonId)
{
	string buttonStr = "";
	switch (buttonId)
	{
	case ID_BUTTON_GRID:
		buttonStr = gToggleButton_Grid ?        "Grid     ( ON  )" : "Grid     ( OFF )";
		break;
     
	case ID_BUTTON_CIRCLE:
		buttonStr = gToggleButton_Circle ?      "Circles  ( ON  )" : "Circles  ( OFF )";
		break;

	case ID_BUTTON_ROI:
		buttonStr = gToggleButton_ROI ?         "ROI      ( ON  )" : "ROI      ( OFF )";
		break;

	case ID_BUTTON_BEZIER:
		buttonStr = gToggleButton_Bezier ?      "Bezier   ( ON  )" : "Bezier   ( OFF )";
		break;

	case ID_BUTTON_MARGIN_LINES:
		buttonStr = gToggleButton_MarginLines ?  "Margin Lines ( ON  )" : "Margin Lines ( OFF )";
		break;

	case ID_BUTTON_DETECTED_LANE:
		buttonStr = gToggleButton_DetectedLane ? "Detected Lane ( ON  )" : "Detected Lane ( OFF )";
		break;

	default:
		break;
	}

	char text[256] = {0};
	strcpy(text, buttonStr.c_str());
    static wchar_t wtext[256] = {0};
	mbstowcs(wtext, text, strlen(text) + 1);//Plus null
	LPWSTR ptr = wtext;
	return ptr;
}


// WinMain: The Application Entry Point
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PWSTR nCmdLine, int nCmdShow)
int WINAPI WinMain( HINSTANCE hInstance,    // HANDLE TO AN INSTANCE.  This is the "handle" to YOUR PROGRAM ITSELF.
                    HINSTANCE hPrevInstance,// USELESS on modern windows (totally ignore hPrevInstance)
                    LPSTR szCmdLine,        // Command line arguments.  similar to argv in standard C programs
                    int iCmdShow )          // Start window maximized, minimized, etc.
{
	// Register the window class
	const wchar_t CLASS_NAME[] = L"WindowClass";
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hInstance = hInstance;
	RegisterClass(&wc);
	// Create the window
	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Display",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT, 
		200,
		400,
		NULL, NULL, hInstance, NULL);

	if (hwnd == 0)
		return 0;

	hndl_Grid = CreateWindow(L"BUTTON", getButtonString(ID_BUTTON_GRID), BS_TEXT | WS_CHILD | WS_VISIBLE, 10, 10, 150, 40, hwnd, (HMENU)ID_BUTTON_GRID, hInstance, NULL);
	hndl_Circle = CreateWindow(L"BUTTON", getButtonString(ID_BUTTON_CIRCLE), BS_TEXT | WS_CHILD | WS_VISIBLE,   10, 60, 150, 40, hwnd, (HMENU)ID_BUTTON_CIRCLE, hInstance, NULL);
	hndl_ROI =    CreateWindow(L"BUTTON", getButtonString(ID_BUTTON_ROI), BS_TEXT | WS_CHILD | WS_VISIBLE,   10, 110, 150, 40, hwnd, (HMENU)ID_BUTTON_ROI, hInstance, NULL);
	hndl_Bezier = CreateWindow(L"BUTTON", getButtonString(ID_BUTTON_BEZIER), BS_TEXT | WS_CHILD | WS_VISIBLE,   10, 160, 150, 40, hwnd, (HMENU)ID_BUTTON_BEZIER, hInstance, NULL);
	hndl_MarginLines =   CreateWindow(L"BUTTON", getButtonString(ID_BUTTON_MARGIN_LINES), BS_TEXT | WS_CHILD | WS_VISIBLE,   10, 210, 150, 40, hwnd, (HMENU)ID_BUTTON_MARGIN_LINES, hInstance, NULL);
	hndl_DetectedLane = CreateWindow(L"BUTTON", getButtonString(ID_BUTTON_DETECTED_LANE), BS_TEXT | WS_CHILD | WS_VISIBLE, 10, 260, 150, 40, hwnd, (HMENU)ID_BUTTON_DETECTED_LANE, hInstance, NULL);


	// Show the window
	ShowWindow(hwnd, iCmdShow);
	iCmdShow = 1;

	std::thread t1(main, "hello");

	// The Message loop
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
// Window Procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:PostQuitMessage(0); return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 5));
		EndPaint(hwnd, &ps);
	}return 0;

	case WM_COMMAND:
	{
		if (LOWORD (wParam) == ID_BUTTON_GRID)
		{
			gToggleButton_Grid = gToggleButton_Grid ? false : true;
			::SetWindowText(hndl_Grid, getButtonString(ID_BUTTON_GRID));
			
		}
		else if (LOWORD(wParam) == ID_BUTTON_CIRCLE)
		{
			gToggleButton_Circle = gToggleButton_Circle ? false : true;
			::SetWindowText(hndl_Circle, getButtonString(ID_BUTTON_CIRCLE));
		}
		else if (LOWORD(wParam) == ID_BUTTON_ROI)
		{
			gToggleButton_ROI = gToggleButton_ROI ? false : true;
			::SetWindowText(hndl_ROI, getButtonString(ID_BUTTON_ROI));
		}
		else if (LOWORD(wParam) == ID_BUTTON_BEZIER)
		{
			gToggleButton_Bezier = gToggleButton_Bezier ? false : true;
			::SetWindowText(hndl_Bezier, getButtonString(ID_BUTTON_BEZIER));
		}
		else if (LOWORD(wParam) == ID_BUTTON_MARGIN_LINES)
		{
			gToggleButton_MarginLines = gToggleButton_MarginLines ? false : true;
			::SetWindowText(hndl_MarginLines, getButtonString(ID_BUTTON_MARGIN_LINES));
		}
		else if (LOWORD(wParam) == ID_BUTTON_DETECTED_LANE)
		{
			gToggleButton_DetectedLane = gToggleButton_DetectedLane ? false : true;
			::SetWindowText(hndl_DetectedLane, getButtonString(ID_BUTTON_DETECTED_LANE));
		}		

	}return 0;

	case WM_CLOSE:
	{
		//if (MessageBox(hwnd, L"Do you want to exit?", L"EXIT", MB_OKCANCEL) == IDOK)
		DestroyWindow(hwnd);
	}return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); // Default Message Handling
}
#endif
