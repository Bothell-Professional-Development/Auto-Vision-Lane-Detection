#include "stdafx.h"

#include "Composition.h"
#include "System.h"

Composition::Composition() :
    m_screenWidth(0),
    m_screenHeight(0),
    m_d3dInterface(NULL),
    m_frameCapture(NULL),
    m_input(NULL),
    m_shaderManager(NULL),
    m_timer(NULL){}
Composition::Composition(const Composition& other){}
Composition::~Composition(){}

bool Composition::Initialize(const bool fullScreen,
                             const bool vSyncEnabled,
                             const unsigned int screenWidth,
                             const unsigned int screenHeight,
                             const float screenDepth,
                             const float screenNear,
                             const D3D11_FILTER samplerFilteringType,
                             const unsigned int maxAnisotropy)
{
    bool success = true;

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    m_d3dInterface = new D3DInterface();

    if(!m_d3dInterface)
    {
        success = false;
    }
    else
    {
        success = m_d3dInterface->Initialize(fullScreen,
                                             vSyncEnabled,
                                             m_screenWidth,
                                             m_screenHeight,
                                             screenDepth,
                                             screenNear);
    }

    if(success)
    {
        m_frameCapture = new RenderTexture();

        success = m_frameCapture->Initialize(m_d3dInterface->GetDevice(),
                                             m_screenWidth,
                                             m_screenHeight);
    }

    if(success)
    {
        m_input = new Input();

        success = m_input->Initialize(System::GetInstance().GetApplicationHandle(),
                                      m_screenWidth,
                                      m_screenHeight);
    }

    if(success)
    {
        m_shaderManager = new ShaderManager();
        success = m_shaderManager->Initialize(m_d3dInterface->GetDevice(),
                                              samplerFilteringType,
                                              maxAnisotropy);
    }

    if(success)
    {
        m_timer = new Timer();
        m_timer->Initialize();
    }

    return success;
}

void Composition::Shutdown()
{
    if(m_d3dInterface)
    {
        m_d3dInterface->Shutdown();
        delete m_d3dInterface;
        m_d3dInterface = NULL;
    }

    if(m_frameCapture)
    {
        m_frameCapture->Shutdown();
        delete m_frameCapture;
        m_frameCapture = NULL;
    }

    if(m_input)
    {
        m_input->Shutdown();
        delete m_input;
        m_input = NULL;
    }

    if(m_shaderManager)
    {
        m_shaderManager->Shutdown();
        delete m_shaderManager;
        m_shaderManager = NULL;
    }

    if(m_timer)
    {
        delete m_timer;
        m_timer = NULL;
    }
}

bool Composition::Frame()
{
    m_timer->Frame();

    bool success = m_input->Frame();

    if(m_input->IsKeyPressed(DIK_ESCAPE))
    {
        success = false;
    }

    if(m_input->IsKeyPressed(DIK_F12))
    {
        success = TakeScreenShot();
    }

    return success;
}

bool Composition::CaptureFrame()
{
    //set the rendering pipeline output to m_frameCapture
    m_frameCapture->SetAsRenderTarget(m_d3dInterface->GetDeviceContext(),
                                      m_d3dInterface->GetDepthStencilView());

    m_frameCapture->ClearTextureData(m_d3dInterface->GetDeviceContext(),
                                      m_d3dInterface->GetDepthStencilView(),
                                      DirectX::XMFLOAT4(0.0f,
                                                        0.0f,
                                                        0.0f,
                                                        1.0f));

    bool success = RenderScene();

    //set back buffer as render target again
    m_d3dInterface->SetBackBufferAsRenderTarget();
    
    if(success)
    {
        //requisite manual step to copy texture data to raster buffer, there isn't
        //really a good way to automate this as Render() indirectly modified m_screenCapture
        success = m_frameCapture->WriteTextureToRaster(m_d3dInterface->GetDevice(),
                                                       m_d3dInterface->GetDeviceContext());
    }

    return success;
}

bool Composition::TakeScreenShot()
{
    bool success = CaptureFrame();

    if(success)
    {
        //TODO: Get filecounter for directory and append to filename

        success = m_frameCapture->WriteTextureToFile(m_d3dInterface->GetDevice(),
                                                     m_d3dInterface->GetDeviceContext(),
                                                     "./screenshot.tga");
    }

    return success;
}