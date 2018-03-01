#include"stdafx.h"

#include "System.h"

LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT retVal = 0;

    switch(message)
    {
        // Check if the window is being destroyed.
        case WM_DESTROY:
            PostQuitMessage(0);

        // Check if the window is being closed.
        case WM_CLOSE:
            PostQuitMessage(0);

        // All other messages pass to the message handler in the system class.
        default:
            retVal = System::GetInstance().MessageHandler(handle, message, wParam, lParam);
    }

    return retVal;
}

bool System::Initialize(Composition* composition,
                        const bool fullScreen,
                        const bool vSync,
                        const unsigned int screenWidth,
                        const unsigned int screenHeight,
                        const float screenDepth,
                        const float screenNear,
                        const D3D11_FILTER samplerFilteringType,
                        const unsigned int maxAnisotropy)
{
    bool success = true;

    m_fullScreen = fullScreen;
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    InitalizeWindows();

    if(m_windowHandle)
    {
        //m_composition = new DrivingSimulation();
        m_composition = composition;
        success = m_composition->Initialize(fullScreen,
                                            vSync,
                                            screenWidth,
                                            screenHeight,
                                            screenDepth,
                                            screenNear,
                                            samplerFilteringType,
                                            maxAnisotropy);
    }
    //if(m_windowHandle)
    //{
    //    m_graphics = new Graphics(fullScreen,
    //                              vSync,
    //                              screenDepth,
    //                              screenNear);
    //    success = m_graphics->Initialize(screenWidth, screenHeight);
    //}
    else
    {
        success = false;
    }

    //if(success)
    //{
    //    m_input = new Input();

    //    success = m_input->Initialize(m_applicationHandle,
    //                                  screenWidth,
    //                                  screenHeight);
    //}

    return success;
}

void System::Shutdown()
{
    ShutdownWindows();

    //if(m_composition)
    //{
    //    m_composition->Shutdown();
    //    delete m_composition;
    //    m_composition = NULL;
    //}

    //if(m_input)
    //{
    //    m_input->Shutdown();
    //    delete m_input;
    //    m_input = NULL;
    //}

    //if(m_graphics)
    //{
    //    m_graphics->Shutdown();
    //    delete m_graphics;
    //    m_graphics = NULL;
    //}
}

void System::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    bool success = true;
    bool finished = false;

    while(!finished)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
        {
            finished = true;
        }
        else
        {
            success = Frame();

            if(!success)
            {
                finished = true;
            }
        }

        //if(m_input->IsKeyPressed(DIK_ESCAPE))
        //{
        //    finished = true;
        //}
    }
}

const HINSTANCE& System::GetApplicationHandle() const
{
    return m_applicationHandle;
}

const HWND& System::GetWindowHandle() const
{
    return m_windowHandle;
}

void System::ShowMessage(const WCHAR* message,
                         const WCHAR* type)
{
    ShowCursor(true);
    MessageBox(m_windowHandle, message, type, MB_OK);
    ShowCursor(false);
}

LRESULT CALLBACK System::MessageHandler(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(handle, message, wParam, lParam);
}


bool System::Frame()
{
    //bool retVal = m_input->Frame();

    //if(retVal)
    //{
    //    retVal = m_graphics->Frame();
    //}

    //return retVal;
    return m_composition->Frame();
}

void System::InitalizeWindows()
{
    m_applicationHandle = GetModuleHandle(NULL);
    m_applicationName = L"Lane Detection Demo";

    WNDCLASSEX windowsClass;
    windowsClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowsClass.lpfnWndProc   = WndProc;
    windowsClass.cbClsExtra    = 0;
    windowsClass.cbWndExtra    = 0;
    windowsClass.hInstance     = m_applicationHandle;
    windowsClass.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    windowsClass.hIconSm       = windowsClass.hIcon;
    windowsClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    windowsClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    windowsClass.lpszMenuName  = NULL;
    windowsClass.lpszClassName = m_applicationName;
    windowsClass.cbSize        = sizeof(WNDCLASSEX);

    RegisterClassEx(&windowsClass);

    DEVMODE screenSettings;
    unsigned int posX = 0;
    unsigned int posY = 0;

    if(m_fullScreen)
    {
        memset(&screenSettings, 0, sizeof(screenSettings));

        screenSettings.dmSize = sizeof(screenSettings);
        screenSettings.dmPelsWidth = GetSystemMetrics(SM_CXSCREEN);
        screenSettings.dmPelsHeight = GetSystemMetrics(SM_CYSCREEN);
        screenSettings.dmBitsPerPel = 32;
        screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN);
    }
    else
    {
        posX = (GetSystemMetrics(SM_CXSCREEN) - m_screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - m_screenHeight) / 2;
    }

    m_windowHandle = CreateWindowEx(WS_EX_APPWINDOW,
                                    m_applicationName,
                                    m_applicationName,
                                    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                                    posX,
                                    posY,
                                    m_screenWidth,
                                    m_screenHeight,
                                    NULL,
                                    NULL,
                                    m_applicationHandle,
                                    NULL);

    ShowWindow(m_windowHandle, SW_SHOW);
    SetForegroundWindow(m_windowHandle);
    SetFocus(m_windowHandle);

    ShowCursor(false);
}

void System::ShutdownWindows()
{
    ShowCursor(true);

    if(m_fullScreen)
    {
        ChangeDisplaySettings(NULL, 0);
    }

    DestroyWindow(m_windowHandle);
    m_windowHandle = NULL;

    UnregisterClass(m_applicationName, m_applicationHandle);
    m_applicationHandle = NULL;
}