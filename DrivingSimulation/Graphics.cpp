#include"stdafx.h"

#include "Graphics.h"

Graphics::Graphics(const bool fullScreen, const bool vSync, const float screenDepth, const float screenNear) :
    m_fullScreen(fullScreen),
    m_vSyncEnabled(vSync),
    m_screenDepth(screenDepth),
    m_screenNear(screenNear),
    m_d3dInterface(NULL),
    m_shaderManager(NULL),
    m_camera(NULL),
    m_model(NULL),
    m_light(NULL)
{
}

Graphics::Graphics(const Graphics& other) {}
Graphics::~Graphics() {}

bool Graphics::Initialize(const int screenWidth,
                          const int screenHeight,
                          const D3D11_FILTER samplerFilteringType,
                          const unsigned int maxAnisotropy)
{
    bool success = true;

    m_d3dInterface = new D3DInterface();

    if(!m_d3dInterface)
    {
        success = false;
    }
    else
    {
        success = m_d3dInterface->Initialize(m_fullScreen,
                                             m_vSyncEnabled,
                                             screenWidth,
                                             screenHeight,
                                             m_screenDepth,
                                             m_screenNear);
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
        m_camera = new Camera();
        m_camera->SetPosition(0.0f, -0.0f, -3.0f);

        m_light = new Light();
        m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
        m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_light->SetDirection(0.0f, 0.25f, 1.0f);
        m_light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_light->SetSpecularPower(32.0f);

        m_model = new Model();
        success = m_model->Initialize(m_d3dInterface->GetDevice(),
                                      m_d3dInterface->GetDeviceContext(),
                                      "./Models/cube.obj",
                                      "./Textures/brick.tga",
                                      Texture::TextureImageFormat::TARGA);
        //success = m_model->Initialize(m_d3dInterface->GetDevice());
    }

    return success;
}

void Graphics::Shutdown()
{
    if(m_d3dInterface)
    {
        m_d3dInterface->Shutdown();
        delete m_d3dInterface;
        m_d3dInterface = NULL;
    }

    if(m_shaderManager)
    {
        m_shaderManager->Shutdown();
        delete m_shaderManager;
        m_shaderManager = NULL;
    }

    if(m_camera)
    {
        delete m_camera;
        m_camera = NULL;
    }

    if(m_light)
    {
        delete m_light;
        m_light = NULL;
    }

    if(m_model)
    {
        m_model->Shutdown();
        delete m_model;
        m_model = NULL;
    }
}

bool Graphics::Frame()
{
    bool retVal = true;

    static float rotation = 0.0f;
    rotation += DirectX::XM_PI * 0.01f;

    if(rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    retVal = Render(rotation);
    
    return retVal;
}

bool Graphics::IsFullScreen() { return m_fullScreen; }
bool Graphics::IsVSyncEnabled() { return m_vSyncEnabled; }
float Graphics::GetScreenDepth() { return m_screenDepth; }
float Graphics::GetScreenNear() { return m_screenNear; }

bool Graphics::Render(const float rotation)
{
    bool success = true;

    m_d3dInterface->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_camera->Render();

    DirectX::XMMATRIX world = m_d3dInterface->GetWorldMatrix();
    DirectX::XMMATRIX view = m_camera->GetViewMatrix();
    DirectX::XMMATRIX projection = m_d3dInterface->GetProjectionMatrix();

    m_model->Render(m_d3dInterface->GetDeviceContext());

    if(m_model->GetTexture())
    {
        if(!m_shaderManager->RenderWithLightShader(m_d3dInterface->GetDeviceContext(),
                                                   m_model->GetIndexCount(),
                                                   world*DirectX::XMMatrixRotationY(rotation) *DirectX::XMMatrixRotationZ(.5f*rotation)*DirectX::XMMatrixRotationX(.5f*rotation),
                                                   view,
                                                   projection,
                                                   m_model->GetTexture(),
                                                   m_light->GetDirection(),
                                                   m_camera->GetPosition(),
                                                   m_light->GetAmbientColor(),
                                                   m_light->GetDiffuseColor(),
                                                   m_light->GetSpecularColor(),
                                                   m_light->GetSpecularPower()))
        {
            success = false;
        }
    }
    else
    {
        if(!m_shaderManager->RenderWithColorShader(m_d3dInterface->GetDeviceContext(),
                                                   m_model->GetIndexCount(),
                                                   world,
                                                   view,
                                                   projection))
        {
            success = false;
        }
    }

    m_d3dInterface->EndScene();

    return success;
}