#include "stdafx.h"

#include <iostream>
#include <thread>
#include <shellapi.h>

#include "System.h"
#include "Timer.h"

//EBLaneDetector include
#include "main.h"

void LaneDetectionWorkLoop(DrivingSimulation* simulation,
                           const float p,
                           const float i,
                           const float d)
{
    unsigned char* rgbBuffer = new unsigned char[simulation->GetTrackCapture()->GetRasterSize()];
    
    CExampleExport detector;
    //detector.Initialize(360.0f,
    //                    -360.0f,
    //                    0.5f,
    //                    0.3f,
    //                    0.0025f); //good

    //Generally very stable but lacks authority around corners. Gets dangerously close to departing lane while turning
    //detector.Initialize(true,
    //                    360.0f,
    //                    -360.0f,
    //                    0.5f,
    //                    0.075f,
    //                    0.0025f);


    //stable @ 45mph with 20ish fps with better cornering performance, slight oscillation around corners and several bounces on returns to straightaways
    //detector.Initialize(true,
    //                    360.0f,
    //                    -360.0f,
    //                    0.7f,
    //                    0.1475f,
    //                    0.015f);

    detector.Initialize(true,
                        360.0f,
                        -360.0f,
                        p,
                        d,
                        i);

    Timer t;
    t.Initialize();

    std::vector<float> steeringCorrections;

    while(true)
    {
        //std::cout << "Workloop executed!\n" << std::endl;
        if(simulation->GetTrackCapture())
        {
            if(simulation->IsRenderingTrackToTexture())
            {
                simulation->GetTrackRasterCopy(rgbBuffer);

                float steeringCorrection = static_cast<float>(*detector.DetectLanes(rgbBuffer,
                                                                                    simulation->GetTrackCapture()->GetHeight(),
                                                                                    simulation->GetTrackCapture()->GetWidth(),
                                                                                    t.GetTimeSeconds()));

                steeringCorrections.push_back(steeringCorrection);
                
                if(steeringCorrections.size() > 5)
                {
                    steeringCorrections.erase(steeringCorrections.begin());
                }


                if(simulation->GetDrivingMovementControls())
                {
                    float average = 0.0f;

                    for(float f : steeringCorrections)
                    {
                        average += f;
                    }

                    simulation->GetDrivingMovementControls()->SetDesiredSteeringPosition(DirectX::XMConvertToRadians(-average / steeringCorrections.size()));
                    //simulation->GetDrivingMovementControls()->SetDesiredSteeringPosition(DirectX::XMConvertToRadians(-steeringCorrection));
                    //simulation->GetDrivingMovementControls()->SetDesiredSteeringPosition(-steeringCorrection);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }

        t.Frame();
    }

    delete[] rgbBuffer;
}

int WINAPI WinMain(HINSTANCE instance,
                   HINSTANCE prevInstance,
                   PSTR cmdLine,
                   int cmdShow)
{
    
    int numArgs;
    LPWSTR* argList = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    
    //PID defaults
    float p = 0.7f;
    float i = 0.015f;
    float d = 0.1475f;

    if(numArgs > 2)
    {
        p = static_cast<float>(_wtof(argList[1]));
        i = static_cast<float>(_wtof(argList[2]));
        d = static_cast<float>(_wtof(argList[3]));
    }

    bool fullScreen = false;
    bool vSync = true;
    unsigned int screenWidth = 1600;
    unsigned int screenHeight = 900;
    float screenDepth = 1000.0f;
    float screenNear = 0.1f;

    DrivingSimulation composition;

    bool result = System::GetInstance().Initialize(&composition,
                                                   fullScreen,
                                                   vSync,
                                                   screenWidth,
                                                   screenHeight,
                                                   screenDepth,
                                                   screenNear,
                                                   D3D11_FILTER_ANISOTROPIC,
                                                   8);

    int exitCode = 0;

    if (result)
    {
        std::thread workloop(LaneDetectionWorkLoop, &composition, p, i, d);
        workloop.detach();

        System::GetInstance().Run();
    }
    else
    {
        exitCode = 1;
    }

    System::GetInstance().Shutdown();

    return exitCode;
}