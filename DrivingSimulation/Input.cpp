#include"stdafx.h"

#include "Input.h"
#include "System.h"

Input::Input():
    m_directInput(NULL),
    m_keyboard(NULL),
    m_mouse(NULL),
    m_screenWidth(0),
    m_screenHeight(0),
    m_mouseX(0),
    m_mouseY(0){}
Input::Input(const Input& other){}
Input::~Input(){}

bool Input::Initialize(HINSTANCE hInstance,
                       const unsigned int screenWidth,
                       const unsigned int screenHeight)
{
    bool success = true;

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    if(FAILED(DirectInput8Create(hInstance,
                                 DIRECTINPUT_VERSION,
                                 IID_IDirectInput8,
                                 (void**)&m_directInput,
                                 NULL)))
    {
        success = false;
    }

    if(success && FAILED(m_directInput->CreateDevice(GUID_SysKeyboard,
                                                     &m_keyboard,
                                                     NULL)))
    {
        success = false;
    }

    if(success && FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)))
    {
        success = false;
    }

    if(success && FAILED(m_keyboard->SetCooperativeLevel(System::GetInstance().GetWindowHandle(),
                                                         DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
    {
        success = false;
    }

    if(success && FAILED(m_keyboard->Acquire()))
    {
        success = false;
    }

    if(success && FAILED(m_directInput->CreateDevice(GUID_SysMouse,
                                                     &m_mouse,
                                                     NULL)))
    {
        success = false;
    }

    if(success && FAILED(m_mouse->SetDataFormat(&c_dfDIMouse)))
    {
        success = false;
    }

    if(success && FAILED(m_mouse->SetCooperativeLevel(System::GetInstance().GetWindowHandle(),
                                                      DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
    {
        success = false;
    }

    if(success && FAILED(m_mouse->Acquire()))
    {
        success = false;
    }

    if(!success)
    {
        System::GetInstance().ShowMessage(L"Input::Initialize: Failed to acquire input devices",
                                          L"Error");
    }

    return success;
}

void Input::Shutdown()
{
    if(m_directInput)
    {
        m_directInput->Release();
        m_directInput = NULL;
    }

    if(m_keyboard)
    {
        m_keyboard->Unacquire();
        m_keyboard->Release();
        m_keyboard = NULL;
    }

    if(m_mouse)
    {
        m_mouse->Unacquire();
        m_mouse->Release();
        m_mouse = NULL;
    }
}

bool Input::Frame()
{
    bool success = ReadKeyboard() && ReadMouse();
    
    if(success)
    {
        ProcessInput();
    }

    return success;
}

bool Input::IsKeyPressed(const unsigned char key)
{
    return m_keyboardState[key] & 0x80 ? true : false;
}

DirectX::XMUINT2 Input::GetMouseLocation()
{
    return DirectX::XMUINT2(m_mouseX, m_mouseY);
}

bool Input::ReadKeyboard()
{
    bool success = true;

    HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState),
                                                (LPVOID)&m_keyboardState);

    if(FAILED(result))
    {
        if(result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
        {
            m_keyboard->Acquire();
        }
        else
        {
            success = false;
        }
    }

    return success;
}

bool Input::ReadMouse()
{
    bool success = true;

    HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE),
                                             (LPVOID)&m_mouseState);


    if(FAILED(result))
    {
        if(result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
        {
            m_mouse->Acquire();
        }
        else
        {
            success = false;
        }
    }

    return success;
}

void Input::ProcessInput()
{
    m_mouseX += m_mouseState.lX;
    m_mouseY += m_mouseState.lY;
}