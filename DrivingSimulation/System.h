#pragma once

#include "stdafx.h"

#include <windows.h>

#include "DrivingSimulation.h"
//#include "Input.h"
//#include "Graphics.h"

static LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

class System
{
public:
    static System& GetInstance()
    {
        static System instance;
        return instance;
    }

    System(System const&) = delete;
    void operator=(System const&) = delete;

    bool Initialize(Composition* composition,
                    const bool fullScreen,
                    const bool vSync,
                    const unsigned int screenWidth,
                    const unsigned int screenHeight,
                    const float screenDepth,
                    const float screenNear,
                    const D3D11_FILTER samplerFilteringType,
                    const unsigned int maxAnisotropy);
    void Shutdown();
    void Run();

    const HINSTANCE& GetApplicationHandle() const;
    const HWND& GetWindowHandle() const;
    void ShowMessage(const WCHAR* message,
                     const WCHAR* type);

    LRESULT CALLBACK MessageHandler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

private:
    System() :
        m_fullScreen(false),
        m_screenWidth(0),
        m_screenHeight(0),
        m_composition(NULL){}
        //m_input(NULL),
        //m_graphics(NULL){}

    bool Frame();
    void InitalizeWindows();
    void ShutdownWindows();


    bool m_fullScreen;
    unsigned int m_screenWidth;
    unsigned int m_screenHeight;    

    LPCWSTR m_applicationName;
    HINSTANCE m_applicationHandle;
    HWND m_windowHandle;

    Composition* m_composition;
    //Input* m_input;
    //Graphics* m_graphics;
};