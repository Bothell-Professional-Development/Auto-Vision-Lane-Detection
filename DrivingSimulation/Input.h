#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <DirectXMath.h>

class Input
{
public:
    Input();
    Input(const Input& other);
    ~Input();

    bool Initialize(HINSTANCE hInstance,
                    const unsigned int screenWidth,
                    const unsigned int screenHeight);

    void Shutdown();
    bool Frame();
    bool IsKeyPressed(const unsigned char key);
    DirectX::XMUINT2 GetMouseLocation();

private:
    bool ReadKeyboard();
    bool ReadMouse();
    void ProcessInput();

    IDirectInput8* m_directInput;
    IDirectInputDevice8* m_keyboard;
    IDirectInputDevice8* m_mouse;

    unsigned char m_keyboardState[256];
    DIMOUSESTATE m_mouseState;

    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
    unsigned int m_mouseX;
    unsigned int m_mouseY;
};