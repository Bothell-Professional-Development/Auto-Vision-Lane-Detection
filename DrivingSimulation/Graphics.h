#pragma once

#include <windows.h>

#include "Camera.h"
#include "D3dInterface.h"
#include "Light.h"
#include "Model.h"
#include "ShaderManager.h"

class Graphics
{
public:
    Graphics(const bool fullScreen, const bool vSync, const float screenDepth, const float screenNear);
    Graphics(const Graphics& other);
    ~Graphics();

    bool Initialize(const int screenWidth,
                    const int screenHeight,
                    const D3D11_FILTER samplerFilteringType,
                    const unsigned int maxAnisotropy);

    void Shutdown();
    bool Frame();

    bool IsFullScreen();
    bool IsVSyncEnabled();
    float GetScreenDepth();
    float GetScreenNear();

private:
    bool Render(const float rotation);

    bool m_fullScreen;
    bool m_vSyncEnabled;
    float m_screenDepth;
    float m_screenNear;

    D3DInterface* m_d3dInterface;
    ShaderManager* m_shaderManager;
    Camera* m_camera;
    Light* m_light;
    Model* m_model;
};