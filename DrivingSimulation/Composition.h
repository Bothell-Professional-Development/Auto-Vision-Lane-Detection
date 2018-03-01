#pragma once

#include <Windows.h>

#include "D3DInterface.h"
#include "Input.h"
#include "RenderTexture.h"
#include "ShaderManager.h"
#include "Timer.h"

class Composition
{
public:
    Composition();
    Composition(const Composition& other);
    ~Composition();

    virtual bool Initialize(const bool fullScreen,
                            const bool vSyncEnabled,
                            const unsigned int screenWidth,
                            const unsigned int screenHeight,
                            const float screenDepth,
                            const float screenNear,
                            const D3D11_FILTER samplerFilteringType,
                            const unsigned int maxAnisotropy);

    virtual void Shutdown();
    virtual bool Frame();

protected:
    virtual bool Render() = 0;
    virtual bool RenderScene() = 0;

    unsigned int m_screenWidth;
    unsigned int m_screenHeight;

    D3DInterface* m_d3dInterface;
    RenderTexture* m_frameCapture;
    Input* m_input;
    ShaderManager* m_shaderManager;
    Timer* m_timer;

private:
    bool CaptureFrame();
    bool TakeScreenShot();
};