#include "stdafx.h"

#include "DrivingSimulation.h"

DrivingSimulation::DrivingSimulation() :
    m_renderTrackToTexture(false),
    m_controls(NULL),
    m_player(NULL),
    m_trackCapture(NULL),
    m_steeringWheel(NULL),
    m_light(NULL),
    m_skyDome(NULL),
    m_terrain(NULL),
    m_track(NULL),
    m_debugDisplay(NULL){}
DrivingSimulation::DrivingSimulation(const DrivingSimulation& other){}
DrivingSimulation::~DrivingSimulation(){}

bool DrivingSimulation::Initialize(const bool fullScreen,
                                   const bool vSync,
                                   const unsigned int screenWidth,
                                   const unsigned int screenHeight,
                                   const float screenDepth,
                                   const float screenNear,
                                   const D3D11_FILTER samplerFilteringType,
                                   const unsigned int maxAnisotropy)
{
    bool success = Composition::Initialize(fullScreen,
                                           vSync,
                                           screenWidth,
                                           screenHeight,
                                           screenDepth,
                                           screenNear,
                                           samplerFilteringType,
                                           maxAnisotropy);

    m_controls = new DrivingMovementControls();
    m_player = new Player();

    success &= m_controls->Initialize(m_input) && m_player->Initialize(m_controls);

    m_player->GetPosition()->SetPosition(0.0f, 2.0f, -1.75f);
    m_player->GetPosition()->SetRotation(DirectX::XMConvertToRadians(5.0f),
                                            DirectX::XMConvertToRadians(90.f), 0.0f);
    m_player->GetMovementControls()->SetMaxForwardVelocity(26.8224f);//53.6448f);
    m_player->GetMovementControls()->SetForwardAcceleration(4.0f);

    m_player->GetCamera()->RenderBaseViewMatrix(0.0f,
                                                0.0f,
                                                -1.0f,
                                                0.0f,
                                                0.0f,
                                                0.0f);

    m_trackCapture = new RenderTexture();

    success &= m_trackCapture->Initialize(m_d3dInterface->GetDevice(),
                                          screenWidth,
                                          screenHeight);

    m_steeringWheel = new Sprite();

    success &= m_steeringWheel->Initialize(m_d3dInterface->GetDevice(),
                                           m_d3dInterface->GetDeviceContext(),
                                           600.0f,
                                           600.0f,
                                           "./Textures/steering_wheel.tga",
                                           Texture::TextureImageFormat::TARGA);
    m_steeringWheel->SetPosition(0.0f, -450.0f, 0.1f);

    m_light = new Light();
    m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->SetDirection(0.3f, -1.0f, 0.3f);
    m_light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_light->SetSpecularPower(100.0f);

    m_skyDome = new SkyDome();
    m_skyDome->Initialize(m_d3dInterface->GetDevice(),
                          "./Models/sphere.obj",
                          DirectX::XMFLOAT4(0.0f, 0.0f, 0.9f, 1.0f),
                          DirectX::XMFLOAT4(0.6f, 0.6f, 0.9f, 1.0f));
    m_skyDome->ReverseFaces(m_d3dInterface->GetDevice());

    m_terrain = new Terrain();
    //success = m_terrain->Initialize(m_d3dInterface->GetDevice(),
    //                                m_d3dInterface->GetDeviceContext(),
    //                                1000,
    //                                1000,
    //                                "./Textures/grass.tga",
    //                                Texture::TextureImageFormat::TARGA);
    success = m_terrain->Initialize(m_d3dInterface->GetDevice(),
                                    m_d3dInterface->GetDeviceContext(),
                                    "./Textures/grass.tga",
                                    Texture::TextureImageFormat::TARGA,
                                    "./Textures/height_map.tga",
                                    Texture::TextureImageFormat::TARGA);

    //Texture::ConvertTargaBgrToRgb("./Textures/brick.tga",
    //                              "./Textures/brick2.tga");
    //Texture::ConvertTargaColorToAlpha("./Textures/steering_wheel2.tga",
    //                                  "./Textures/steering_wheel.tga",
    //                                  0xFF,
    //                                  0xFF,
    //                                  0xFF);

    m_track = new Track();

    success &= m_track->Initialize(m_d3dInterface->GetDevice(),
                                   m_d3dInterface->GetDeviceContext(),
                                   3.8f,
                                   "./Tracks/track.txt",
                                   "./Textures/road.tga",
                                   Texture::TextureImageFormat::TARGA);

    //success = m_track->Initialize(m_d3dInterface->GetDevice(),
    //                              4.0f,
    //                              "./Tracks/track.txt",
    //                              DirectX::XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f));

    m_debugDisplay = new DebugDisplay();

    success &= success && m_debugDisplay->Initialize(m_d3dInterface,
                                                     5,
                                                     5,
                                                     16,
                                                     2,
                                                     DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
                                                     1.0f,
                                                     screenWidth,
                                                     screenHeight,
                                                     "./Textures/glyphs.tga",
                                                     Texture::TextureImageFormat::TARGA);

    return success;
}

void DrivingSimulation::Shutdown()
{
    m_renderTrackToTexture = false;

    if(m_controls)
    {
        delete m_controls;
        m_controls = NULL;
    }

    if(m_player)
    {
        m_player->Shutdown();
        delete m_player;
        m_player = NULL;
    }

    if(m_trackCapture)
    {
        m_trackCapture->Shutdown();
        delete m_trackCapture;
        m_trackCapture = NULL;
    }

    if(m_steeringWheel)
    {
        m_steeringWheel->Shutdown();
        delete m_steeringWheel;
        m_steeringWheel = NULL;
    }

    if(m_light)
    {
        delete m_light;
        m_light = NULL;
    }

    if(m_skyDome)
    {
        m_skyDome->Shutdown();
        delete m_skyDome;
        m_skyDome = NULL;
    }

    if(m_terrain)
    {
        m_terrain->Shutdown();
        delete m_terrain;
        m_terrain = NULL;
    }

    if(m_track)
    {
        m_track->Shutdown();
        delete m_track;
        m_track = NULL;
    }

    if(m_debugDisplay)
    {
        m_debugDisplay->Shutdown();
        delete m_debugDisplay;
        m_debugDisplay = NULL;
    }

    Composition::Shutdown();
}

bool DrivingSimulation::Frame()
{
    bool success = Composition::Frame();

    if(m_input->IsKeyPressed(DIK_F11))
    {
        m_renderTrackToTexture = !m_renderTrackToTexture;
    }

    success &= m_debugDisplay->Frame(m_d3dInterface->GetDeviceContext(),
                                     *m_player->GetPosition(),
                                     *m_player->GetMovementControls());

    m_skyDome->SetPosition(m_player->GetPosition()->GetPosition().x,
                           m_player->GetPosition()->GetPosition().y,
                           m_player->GetPosition()->GetPosition().z);

    m_player->Frame(m_timer->GetTimeSeconds());

    //DirectX::XMFLOAT3 currentSteeringRotation = m_steeringWheel->GetRotation();
    //m_steeringWheel->SetRotation(currentSteeringRotation.x,
    //                             currentSteeringRotation.y,
    //                             static_cast<DrivingMovementControls*>(m_player->GetMovementControls())->GetSteeringPosition());

    if(m_renderTrackToTexture)
    {
        success &= RenderTrackToTexture() && Render();
    }
    else
    {
        success &= Render();
    }

    return success;
}

bool DrivingSimulation::SetTrackCaptureResolution(const unsigned int width,
                                                  const unsigned int height)
{
    if(m_trackCapture)
    {
        m_trackCapture->Shutdown();
        delete m_trackCapture;
        m_trackCapture = NULL;
    }

    m_trackCapture = new RenderTexture();

    return m_trackCapture->Initialize(m_d3dInterface->GetDevice(),
                                      width,
                                      height);
}

bool DrivingSimulation::Render()
{
    m_d3dInterface->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);
    bool success = RenderScene();
    m_d3dInterface->EndScene();

    return success;
}

bool DrivingSimulation::RenderScene()
{
    bool success = true;

    Camera* camera = m_player->GetCamera();
    camera->Render();

    DirectX::XMMATRIX world = m_d3dInterface->GetWorldMatrix();
    DirectX::XMMATRIX projection = m_d3dInterface->GetProjectionMatrix();
    DirectX::XMMATRIX ortho = m_d3dInterface->GetOrthoMatrix();
    DirectX::XMMATRIX view = camera->GetViewMatrix();

    ID3D11DeviceContext* context = m_d3dInterface->GetDeviceContext();

    m_d3dInterface->DisableZBuffer();
    m_skyDome->Render(context);

    if(!m_shaderManager->RenderWithSkyDomeShader(context,
                                                 m_skyDome->GetIndexCount(),
                                                 world * DirectX::XMMatrixTranslation(m_skyDome->GetPosition().x,
                                                                                      m_skyDome->GetPosition().y,
                                                                                      m_skyDome->GetPosition().z),
                                                 view,
                                                 projection,
                                                 m_skyDome->GetApexColor(),
                                                 m_skyDome->GetColor(),
                                                 m_skyDome->GetBoundingBoxDimensions().x / 2))
    {
        success = false;
    }
    m_d3dInterface->EnableZBuffer();

    if(success)
    {
        m_terrain->Render(context);

        if(!m_shaderManager->RenderWithLightShader(context,
                                                   m_terrain->GetIndexCount(),
                                                   world,
                                                   view,
                                                   projection,
                                                   m_terrain->GetTexture(),
                                                   m_light->GetDirection(),
                                                   camera->GetPosition(),
                                                   m_light->GetAmbientColor(),
                                                   m_light->GetDiffuseColor(),
                                                   m_light->GetSpecularColor(),
                                                   m_light->GetSpecularPower()))
        {
            success = false;
        }
    }

    if(success)
    {
        m_track->Render(context);

        if(!m_shaderManager->RenderWithLightShader(context,
                                                   m_track->GetIndexCount(),
                                                   world,
                                                   view,
                                                   projection,
                                                   m_track->GetTexture(),
                                                   m_light->GetDirection(),
                                                   camera->GetPosition(),
                                                   m_light->GetAmbientColor(),
                                                   m_light->GetDiffuseColor(),
                                                   m_light->GetSpecularColor(),
                                                   m_light->GetSpecularPower()))
        {
            success = false;
        }
    }

    m_d3dInterface->EnableAlphaBlending();
    if(success)
    {
        success &= m_debugDisplay->Render(context,
                                          m_shaderManager,
                                          world,
                                          camera->GetBaseViewMatrix(),
                                          ortho);

        m_steeringWheel->Render(context);

        DirectX::XMFLOAT3 steeringWheelPosition = m_steeringWheel->GetPosition();

        success &= m_shaderManager->RenderWithTextureShader(context,
                                                            m_steeringWheel->GetIndexCount(),
                                                            world * DirectX::XMMatrixRotationZ(-static_cast<DrivingMovementControls*>(m_player->GetMovementControls())->GetSteeringPosition()),
                                                            camera->GetBaseViewMatrix() * DirectX::XMMatrixTranslation(steeringWheelPosition.x,
                                                                                                                       steeringWheelPosition.y,
                                                                                                                       steeringWheelPosition.z),
                                                            ortho,
                                                            m_steeringWheel->GetTexture());
    }
    m_d3dInterface->DisableAlphaBlending();

    //m_d3dInterface->EnableAlphaBlending();
    //static float rotation = 0.0f;
    //rotation += DirectX::XM_PI * 0.01f;
    //Model* debug = new Model();
    //debug->Initialize(m_d3dInterface->GetDevice(),
    //                  m_d3dInterface->GetDeviceContext(),
    //                  "./Models/sphere.obj",
    //                  "./Textures/steering_wheel.tga",
    //                  Texture::TextureImageFormat::TARGA);
    //debug->Render(m_d3dInterface->GetDeviceContext());
    //m_shaderManager->RenderWithLightShader(context,
    //                                       debug->GetIndexCount(),
    //                                       world*DirectX::XMMatrixRotationY(rotation)*DirectX::XMMatrixTranslation(rotation, 1.0f, 1.0f),
    //                                       view,
    //                                       projection,
    //                                       debug->GetTexture(),
    //                                       m_light->GetDirection(),
    //                                       camera->GetPosition(),
    //                                       m_light->GetAmbientColor(),
    //                                       m_light->GetDiffuseColor(),
    //                                       m_light->GetSpecularColor(),
    //                                       m_light->GetSpecularPower());

    //debug->Shutdown();
    //delete debug;
    //m_d3dInterface->DisableAlphaBlending();

    return success;
}
bool DrivingSimulation::RenderTrackToTexture()
{
    bool success = true;

    //set the rendering pipeline output to m_frameCapture
    m_trackCapture->SetAsRenderTarget(m_d3dInterface->GetDeviceContext(),
                                      m_d3dInterface->GetDepthStencilView());

    m_trackCapture->ClearTextureData(m_d3dInterface->GetDeviceContext(),
                                     m_d3dInterface->GetDepthStencilView(),
                                     DirectX::XMFLOAT4(0.2f,
                                                       0.2f,
                                                       0.2f,
                                                       1.0f));

    Camera* camera = m_player->GetCamera();
    camera->Render();

    DirectX::XMMATRIX world = m_d3dInterface->GetWorldMatrix();
    DirectX::XMMATRIX projection = m_d3dInterface->GetProjectionMatrix();
    DirectX::XMMATRIX view = camera->GetViewMatrix();

    ID3D11DeviceContext* context = m_d3dInterface->GetDeviceContext();

    m_track->Render(context);

    if(FAILED(m_shaderManager->RenderWithLightShader(context,
                                                     m_track->GetIndexCount(),
                                                     world,
                                                     view,
                                                     projection,
                                                     m_track->GetTexture(),
                                                     m_light->GetDirection(),
                                                     camera->GetPosition(),
                                                     m_light->GetAmbientColor(),
                                                     m_light->GetDiffuseColor(),
                                                     m_light->GetSpecularColor(),
                                                     m_light->GetSpecularPower())))
    {
        success = false;
    }

    //set back buffer as render target again
    m_d3dInterface->SetBackBufferAsRenderTarget();

    if(success)
    {
        m_trackCaptureMutex.lock();
        success = m_trackCapture->WriteTextureToRaster(m_d3dInterface->GetDevice(),
                                                       m_d3dInterface->GetDeviceContext());
        //m_trackCapture->WriteTextureToFile(m_d3dInterface->GetDevice(),
        //                                   m_d3dInterface->GetDeviceContext(),
        //                                   "./shit.tga");
        m_trackCaptureMutex.unlock();
    }

    return success;
}

const bool& DrivingSimulation::IsRenderingTrackToTexture() const
{
    return m_renderTrackToTexture;
}

DrivingMovementControls* DrivingSimulation::GetDrivingMovementControls() const
{
    return m_controls;
}

const RenderTexture* DrivingSimulation::GetTrackCapture() const
{
    return m_trackCapture;
}

void DrivingSimulation::GetTrackRasterCopy(unsigned char* copyDestination)
{
    if(m_trackCapture)
    {
        m_trackCaptureMutex.lock();
        memcpy(copyDestination, m_trackCapture->GetRasterData(), m_trackCapture->GetRasterSize());
        m_trackCaptureMutex.unlock();
    }
}