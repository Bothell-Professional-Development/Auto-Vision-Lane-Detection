#include "stdafx.h"

#include <math.h>
#include <sstream>

#include "DebugDisplay.h"

const std::string DebugDisplay::ADAPTER_DESCRIPTION_FIELD_TEXT = "Adapter: ";
const std::string DebugDisplay::ADAPTER_MEMORY_FIELD_TEXT = "Adapter Memory: ";
const std::string DebugDisplay::SCREEN_RESOLUTION_FIELD_TEXT = "Screen Resolution: ";
const std::string DebugDisplay::FRAME_RATE_FIELD_TEXT = "Frame Rate: ";
const std::string DebugDisplay::CPU_UTILIZATION_FIELD_TEXT = "CPU Utilization: ";
const std::string DebugDisplay::X_POSITION_FIELD_TEXT = "X Position: ";
const std::string DebugDisplay::Y_POSITION_FIELD_TEXT = "Y Position: ";
const std::string DebugDisplay::Z_POSITION_FIELD_TEXT = "Z Position: ";
const std::string DebugDisplay::X_ROTATION_FIELD_TEXT = "X Rotation: ";
const std::string DebugDisplay::Y_ROTATION_FIELD_TEXT = "Y Rotation: ";
const std::string DebugDisplay::Z_ROTATION_FIELD_TEXT = "Z Rotation: ";
const std::string DebugDisplay::LON_VELOCITY_FIELD_TEXT = "Longitudinal Velocity: ";
const std::string DebugDisplay::LAT_VELOCITY_FIELD_TEXT = "Lateral Velocity: ";
const std::string DebugDisplay::VERTICAL_VELOCITY_FIELD_TEXT = "Vertical Velocity: ";

const unsigned char DebugDisplay::ADAPTER_DESCRIPTION_VALUE_LENGTH = 128;
const unsigned char DebugDisplay::ADAPTER_MEMORY_VALUE_LENGTH = 7; //iiiiiMB
const unsigned char DebugDisplay::SCREEN_RESOLUTION_VALUE_LENGTH = 9; //iiiixiiii
const unsigned char DebugDisplay::FRAME_RATE_VALUE_LENGTH = 6; //iiifps
const unsigned char DebugDisplay::CPU_UTILIZATION_VALUE_LENGTH = 4; //iii%
const unsigned char DebugDisplay::POSITION_VALUE_LENGTH = 11; //-iiiiiii.dd
const unsigned char DebugDisplay::ROTATION_VALUE_LENGTH = 31; //-i.dd radians (iii.dd degrees)
const unsigned char DebugDisplay::VELOCITY_VALUE_LENGTH = 24; //-iiiiiii.ddm/s (iiiimph)

const float DebugDisplay::MS_TO_MPH = 2.23694f;

DebugDisplay::DebugDisplay() :
    m_adapterDescriptionPositionX(0),
    m_adapterDescriptionPositionY(0),
    m_adapterMemoryPositionX(0),
    m_adapterMemoryPositionY(0),
    m_screenResolutionPositionX(0),
    m_screenResolutionPositionY(0),
    m_frameRatePositionX(0),
    m_frameRatePositionY(0),
    m_processorUtilizationPositionX(0),
    m_processorUtilizationPositionY(0),
    m_xPositionX(0),
    m_xPositionY(0),
    m_yPositionX(0),
    m_yPositionY(0),
    m_zPositionX(0),
    m_zPositionY(0),
    m_xRotationPositionX(0),
    m_xRotationPositionY(0),
    m_yRotationPositionX(0),
    m_yRotationPositionY(0),
    m_zRotationPositionX(0),
    m_zRotationPositionY(0),
    m_lonVelocityPositionX(0),
    m_lonVelocityPositionY(0),
    m_latVelocityPositionX(0),
    m_latVelocityPositionY(0),
    m_verticalVelocityPositionX(0),
    m_verticalVelocityPositionY(0),
    m_textColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_performanceMetrics(NULL),
    m_adapterDescriptionText(NULL),
    m_screenResolutionText(NULL),
    m_frameRateText(NULL),
    m_processorUtilizationText(NULL),
    m_xPositionText(NULL),
    m_yPositionText(NULL),
    m_zPositionText(NULL),
    m_xRotationText(NULL),
    m_yRotationText(NULL),
    m_zRotationText(NULL),
    m_lonVelocityText(NULL),
    m_latVelocityText(NULL),
    m_verticalVelocityText(NULL){}
DebugDisplay::DebugDisplay(const DebugDisplay& other){}
DebugDisplay::~DebugDisplay(){}

bool DebugDisplay::Initialize(D3DInterface* d3dInterface,
                              const unsigned int positionX,
                              const unsigned int positionY,
                              const unsigned int textHeight,
                              const unsigned int verticalSpacing,
                              const DirectX::XMFLOAT4 textColor,
                              const float updatesPerSecond,
                              const unsigned int screenWidth,
                              const unsigned int screenHeight,
                              const char* textureFilename,
                              const Texture::TextureImageFormat imageFormat)
{
    

    unsigned int verticalPosition = positionY;
    ID3D11Device* device = d3dInterface->GetDevice();
    ID3D11DeviceContext* context = d3dInterface->GetDeviceContext();

    m_performanceMetrics = new PerformanceMetrics();

    m_performanceMetrics->Initialize(updatesPerSecond);

    m_textColor = textColor;
    m_adapterDescriptionPositionX = positionX;
    m_adapterDescriptionPositionY = verticalPosition;
    m_adapterDescriptionText = new Text();

    bool success = m_adapterDescriptionText->Initialize(device,
                                                        context,
                                                        screenWidth,
                                                        screenHeight,
                                                        textureFilename,
                                                        imageFormat);

    success &= m_adapterDescriptionText->InitializeText(device,
                                                        ADAPTER_DESCRIPTION_FIELD_TEXT.length() + 
                                                        ADAPTER_DESCRIPTION_VALUE_LENGTH);

    unsigned int videoMemory;
    const char* adapterDetails = d3dInterface->GetVideoHardwareInfo(videoMemory);

    success &= UpdateAdapterDescriptionText(context, adapterDetails);

    verticalPosition += textHeight + verticalSpacing;
    m_adapterMemoryPositionX = positionX;
    m_adapterMemoryPositionY = verticalPosition;
    m_adapterMemoryText = new Text();

    success &= m_adapterMemoryText->Initialize(device,
                                               context,
                                               screenWidth,
                                               screenHeight,
                                               textureFilename,
                                               imageFormat);

    success &= m_adapterMemoryText->InitializeText(device,
                                                   ADAPTER_MEMORY_FIELD_TEXT.length() +
                                                   ADAPTER_MEMORY_VALUE_LENGTH);
    success &= UpdateAdapterMemoryText(context, videoMemory);

    verticalPosition += textHeight + verticalSpacing;
    m_screenResolutionPositionX = positionX;
    m_screenResolutionPositionY = verticalPosition;
    m_screenResolutionText = new Text();

    success &= m_screenResolutionText->Initialize(device,
                                                  context,
                                                  screenWidth,
                                                  screenHeight,
                                                  textureFilename,
                                                  imageFormat);

    success &= m_screenResolutionText->InitializeText(device,
                                                      SCREEN_RESOLUTION_FIELD_TEXT.length() +
                                                      SCREEN_RESOLUTION_VALUE_LENGTH);
    success &= UpdateScreenResolutionText(context,
                                          screenWidth,
                                          screenHeight);

    verticalPosition += textHeight + verticalSpacing;
    m_frameRatePositionX = positionX;
    m_frameRatePositionY = verticalPosition;
    m_frameRateText = new Text();

    success &= m_frameRateText->Initialize(device,
                                           context,
                                           screenWidth,
                                           screenHeight,
                                           textureFilename,
                                           imageFormat);

    success &= m_frameRateText->InitializeText(device,
                                               FRAME_RATE_FIELD_TEXT.length() +
                                               FRAME_RATE_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_processorUtilizationPositionX = positionX;
    m_processorUtilizationPositionY = verticalPosition;
    m_processorUtilizationText = new Text();

    success &= m_processorUtilizationText->Initialize(device,
                                                      context,
                                                      screenWidth,
                                                      screenHeight,
                                                      textureFilename,
                                                      imageFormat);

    success &= m_processorUtilizationText->InitializeText(device,
                                                          CPU_UTILIZATION_FIELD_TEXT.length() +
                                                          CPU_UTILIZATION_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_xPositionX = positionX;
    m_xPositionY = verticalPosition;
    m_xPositionText = new Text();

    success &= m_xPositionText->Initialize(device,
                                           context,
                                           screenWidth,
                                           screenHeight,
                                           textureFilename,
                                           imageFormat);

    success &= m_xPositionText->InitializeText(device,
                                               X_POSITION_FIELD_TEXT.length() +
                                               POSITION_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_yPositionX = positionX;
    m_yPositionY = verticalPosition;
    m_yPositionText = new Text();

    success &= m_yPositionText->Initialize(device,
                                           context,
                                           screenWidth,
                                           screenHeight,
                                           textureFilename,
                                           imageFormat);

    success &= m_yPositionText->InitializeText(device,
                                               Y_POSITION_FIELD_TEXT.length() +
                                               POSITION_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_zPositionX = positionX;
    m_zPositionY = verticalPosition;
    m_zPositionText = new Text();

    success &= m_zPositionText->Initialize(device,
                                           context,
                                           screenWidth,
                                           screenHeight,
                                           textureFilename,
                                           imageFormat);

    success &= m_zPositionText->InitializeText(device,
                                               Z_POSITION_FIELD_TEXT.length() +
                                               POSITION_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_xRotationPositionX = positionX;
    m_xRotationPositionY = verticalPosition;
    m_xRotationText = new Text();

    success &= m_xRotationText->Initialize(device,
                                           context,
                                           screenWidth,
                                           screenHeight,
                                           textureFilename,
                                           imageFormat);

    success &= m_xRotationText->InitializeText(device,
                                               X_ROTATION_FIELD_TEXT.length() +
                                               ROTATION_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_yRotationPositionX = positionX;
    m_yRotationPositionY = verticalPosition;
    m_yRotationText = new Text();

    success &= m_yRotationText->Initialize(device,
                                           context,
                                           screenWidth,
                                           screenHeight,
                                           textureFilename,
                                           imageFormat);

    success &= m_yRotationText->InitializeText(device,
                                               Y_ROTATION_FIELD_TEXT.length() +
                                               ROTATION_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_zRotationPositionX = positionX;
    m_zRotationPositionY = verticalPosition;
    m_zRotationText = new Text();

    success &= m_zRotationText->Initialize(device,
                                           context,
                                           screenWidth,
                                           screenHeight,
                                           textureFilename,
                                           imageFormat);

    success &= m_zRotationText->InitializeText(device,
                                               Z_ROTATION_FIELD_TEXT.length() +
                                               ROTATION_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_lonVelocityPositionX = positionX;
    m_lonVelocityPositionY = verticalPosition;
    m_lonVelocityText = new Text();

    success &= m_lonVelocityText->Initialize(device,
                                             context,
                                             screenWidth,
                                             screenHeight,
                                             textureFilename,
                                             imageFormat);

    success &= m_lonVelocityText->InitializeText(device,
                                                 LON_VELOCITY_FIELD_TEXT.length() +
                                                 VELOCITY_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_latVelocityPositionX = positionX;
    m_latVelocityPositionY = verticalPosition;
    m_latVelocityText = new Text();

    success &= m_latVelocityText->Initialize(device,
                                             context,
                                             screenWidth,
                                             screenHeight,
                                             textureFilename,
                                             imageFormat);

    success &= m_latVelocityText->InitializeText(device,
                                                 LAT_VELOCITY_FIELD_TEXT.length() +
                                                 VELOCITY_VALUE_LENGTH);

    verticalPosition += textHeight + verticalSpacing;
    m_verticalVelocityPositionX = positionX;
    m_verticalVelocityPositionY = verticalPosition;
    m_verticalVelocityText = new Text();

    success &= m_verticalVelocityText->Initialize(device,
                                                  context,
                                                  screenWidth,
                                                  screenHeight,
                                                  textureFilename,
                                                  imageFormat);

    success &= m_verticalVelocityText->InitializeText(device,
                                                      VERTICAL_VELOCITY_FIELD_TEXT.length() +
                                                      VELOCITY_VALUE_LENGTH);

    if(success)
    {
        m_textList.push_back(m_adapterDescriptionText);
        m_textList.push_back(m_adapterMemoryText);
        m_textList.push_back(m_screenResolutionText);
        m_textList.push_back(m_frameRateText);
        m_textList.push_back(m_processorUtilizationText);
        m_textList.push_back(m_xPositionText);
        m_textList.push_back(m_yPositionText);
        m_textList.push_back(m_zPositionText);
        m_textList.push_back(m_xRotationText);
        m_textList.push_back(m_yRotationText);
        m_textList.push_back(m_zRotationText);
        m_textList.push_back(m_lonVelocityText);
        m_textList.push_back(m_latVelocityText);
        m_textList.push_back(m_verticalVelocityText);
    }

    return success;
}

void DebugDisplay::Shutdown()
{
    if(m_performanceMetrics)
    {
        m_performanceMetrics->Shutdown();
        delete m_performanceMetrics;
        m_performanceMetrics = NULL;
    }

    if(m_adapterDescriptionText)
    {
        m_adapterDescriptionText->Shutdown();
        delete m_adapterDescriptionText;
        m_adapterDescriptionText = NULL;
    }

    if(m_adapterMemoryText)
    {
        m_adapterMemoryText->Shutdown();
        delete m_adapterMemoryText;
        m_adapterMemoryText = NULL;
    }

    if(m_screenResolutionText)
    {
        m_screenResolutionText->Shutdown();
        delete m_screenResolutionText;
        m_screenResolutionText = NULL;
    }

    if(m_frameRateText)
    {
        m_frameRateText->Shutdown();
        delete m_frameRateText;
        m_frameRateText = NULL;
    }

    if(m_processorUtilizationText)
    {
        m_processorUtilizationText->Shutdown();
        delete m_processorUtilizationText;
        m_processorUtilizationText = NULL;
    }

    if(m_xPositionText)
    {
        m_xPositionText->Shutdown();
        delete m_xPositionText;
        m_xPositionText = NULL;
    }

    if(m_yPositionText)
    {
        m_yPositionText->Shutdown();
        delete m_yPositionText;
        m_yPositionText = NULL;
    }

    if(m_zPositionText)
    {
        m_zPositionText->Shutdown();
        delete m_zPositionText;
        m_zPositionText = NULL;
    }

    if(m_xRotationText)
    {
        m_xRotationText->Shutdown();
        delete m_xRotationText;
        m_xRotationText = NULL;
    }

    if(m_yRotationText)
    {
        m_yRotationText->Shutdown();
        delete m_yRotationText;
        m_yRotationText = NULL;
    }

    if(m_zRotationText)
    {
        m_zRotationText->Shutdown();
        delete m_zRotationText;
        m_zRotationText = NULL;
    }

    if(m_lonVelocityText)
    {
        m_lonVelocityText->Shutdown();
        delete m_lonVelocityText;
        m_lonVelocityText = NULL;
    }

    if(m_latVelocityText)
    {
        m_latVelocityText->Shutdown();
        delete m_latVelocityText;
        m_latVelocityText = NULL;
    }

    if(m_verticalVelocityText)
    {
        m_verticalVelocityText->Shutdown();
        delete m_verticalVelocityText;
        m_verticalVelocityText = NULL;
    }

    m_textList.clear();
}

bool DebugDisplay::Frame(ID3D11DeviceContext* context,
                         const Position& position,
                         const MovementControls& movementControls)
{
    m_performanceMetrics->Frame();

    DirectX::XMFLOAT3 pos = position.GetPosition();
    DirectX::XMFLOAT3 rotation = position.GetRotation();
    DirectX::XMFLOAT3 velocity = movementControls.GetVelocity();

    bool success = UpdateFrameRateText(context);
    
    success &= UpdateProcessorUtilizationText(context);

    success &= UpdateXPositionText(context,
                                   pos.x);

    success &= UpdateYPositionText(context,
                                   pos.y);

    success &= UpdateZPositionText(context,
                                   pos.z);

    success &= UpdateXRotationText(context,
                                   rotation.x);

    success &= UpdateYRotationText(context,
                                   rotation.y);

    success &= UpdateZRotationText(context,
                                   rotation.z);

    success &= UpdateLongitudinalVelocityText(context,
                                              velocity.z);

    success &= UpdateLateralVelocityText(context,
                                         velocity.x);

    success &= UpdateVerticalVelocityText(context,
                                          velocity.y);

    return success;
}

bool DebugDisplay::Render(ID3D11DeviceContext* context,
                          ShaderManager* shaderManager,
                          const DirectX::XMMATRIX& world,
                          const DirectX::XMMATRIX& view,
                          const DirectX::XMMATRIX& projection)
{
    bool success = true;

    for(Text* text : m_textList)
    {
        text->Render(context);

        if(FAILED(shaderManager->RenderWithTextShader(context,
                                                      text->GetIndexCount(),
                                                      world,
                                                      view,
                                                      projection,
                                                      text->GetTexture(),
                                                      text->GetColor())))
        {
            success = false;
            break;
        }
    }

    return success;
}

bool DebugDisplay::UpdateAdapterDescriptionText(ID3D11DeviceContext* context,
                                                const char* adapterDescription)
{
    std::stringstream ss;

    ss << ADAPTER_DESCRIPTION_FIELD_TEXT << adapterDescription;

    return m_adapterDescriptionText->UpdateText(context,
                                                ss.str().c_str(),
                                                m_adapterDescriptionPositionX,
                                                m_adapterDescriptionPositionY,
                                                m_textColor);
}

bool DebugDisplay::UpdateAdapterMemoryText(ID3D11DeviceContext* context,
                                           const unsigned int videoMemory)
{
    std::stringstream ss;

    ss << ADAPTER_MEMORY_FIELD_TEXT << videoMemory << "MB";

    return m_adapterMemoryText->UpdateText(context,
                                           ss.str().c_str(),
                                           m_adapterMemoryPositionX,
                                           m_adapterMemoryPositionY,
                                           m_textColor);
}

bool DebugDisplay::UpdateScreenResolutionText(ID3D11DeviceContext* context,
                                              const unsigned int width,
                                              const unsigned int height)
{
    std::stringstream ss;

    ss << SCREEN_RESOLUTION_FIELD_TEXT << width << "x" << height;

    return m_screenResolutionText->UpdateText(context,
                                              ss.str().c_str(),
                                              m_screenResolutionPositionX,
                                              m_screenResolutionPositionY,
                                              m_textColor);
}

bool DebugDisplay::UpdateFrameRateText(ID3D11DeviceContext* context)
{
        std::stringstream ss;

        unsigned int frameRate = m_performanceMetrics->GetFrameRate();
    
        ss << "Frame Rate: " << frameRate << "fps";
    
        DirectX::XMFLOAT4 color;
        color.w = 1.0f;
    
        if(frameRate > 30)
        {
            color.x = 0.0f;
            color.y = 1.0f;
            color.z = 0.0f;
        }
        else if(frameRate < 20)
        {
            color.x = 1.0f;
            color.y = 0.0f;
            color.z = 0.0f;
        }
        else
        {
            color.x = 1.0f;
            color.y = 1.0f;
            color.z = 0.0f;
        }
    
        return m_frameRateText->UpdateText(context,
                                           ss.str().c_str(),
                                           m_frameRatePositionX,
                                           m_frameRatePositionY,
                                           color);
}

bool DebugDisplay::UpdateProcessorUtilizationText(ID3D11DeviceContext* context)
{
    std::stringstream ss;

    unsigned int processorUtilization = m_performanceMetrics->GetProcessorUtilization();

    ss << CPU_UTILIZATION_FIELD_TEXT << processorUtilization << "%";

    DirectX::XMFLOAT4 color;
    color.w = 1.0f;

    if(processorUtilization < 25)
    {
        color.x = 0.0f;
        color.y = 1.0f;
        color.z = 0.0f;
    }
    else if(processorUtilization > 75)
    {
        color.x = 1.0f;
        color.y = 0.0f;
        color.z = 0.0f;
    }
    else
    {
        color.x = 1.0f;
        color.y = 1.0f;
        color.z = 0.0f;
    }

    return m_processorUtilizationText->UpdateText(context,
                                                  ss.str().c_str(),
                                                  m_processorUtilizationPositionX,
                                                  m_processorUtilizationPositionY,
                                                  color);
}

bool DebugDisplay::UpdateXPositionText(ID3D11DeviceContext* context,
                                       const float posX)
{
    std::stringstream ss;

    ss << X_POSITION_FIELD_TEXT << ceilf(posX * 100) / 100;

    return m_xPositionText->UpdateText(context,
                                       ss.str().c_str(),
                                       m_xPositionX,
                                       m_xPositionY,
                                       m_textColor);
}

bool DebugDisplay::UpdateYPositionText(ID3D11DeviceContext* context,
                                       const float posY)
{
    std::stringstream ss;

    ss << Y_POSITION_FIELD_TEXT << ceilf(posY * 100) / 100;

    return m_yPositionText->UpdateText(context,
                                       ss.str().c_str(),
                                       m_yPositionX,
                                       m_yPositionY,
                                       m_textColor);
}

bool DebugDisplay::UpdateZPositionText(ID3D11DeviceContext* context,
                                       const float posZ)
{
    std::stringstream ss;

    ss << Z_POSITION_FIELD_TEXT << ceilf(posZ * 100) / 100;

    return m_zPositionText->UpdateText(context,
                                       ss.str().c_str(),
                                       m_zPositionX,
                                       m_zPositionY,
                                       m_textColor);
}

bool DebugDisplay::UpdateXRotationText(ID3D11DeviceContext* context,
                                       const float rotX)
{
    std::stringstream ss;

    ss << X_ROTATION_FIELD_TEXT << ceilf(rotX * 100) / 100 << " radians ("
       << ceilf(DirectX::XMConvertToDegrees(rotX) * 100) / 100 << " degrees)";

    return m_xRotationText->UpdateText(context,
                                       ss.str().c_str(),
                                       m_xRotationPositionX,
                                       m_xRotationPositionY,
                                       m_textColor);
}

bool DebugDisplay::UpdateYRotationText(ID3D11DeviceContext* context,
                                       const float rotY)
{
    std::stringstream ss;

    ss << Y_ROTATION_FIELD_TEXT << ceilf(rotY * 100) / 100 << " radians ("
       << ceilf(DirectX::XMConvertToDegrees(rotY) * 100) / 100 << " degrees)";

    return m_yRotationText->UpdateText(context,
                                       ss.str().c_str(),
                                       m_yRotationPositionX,
                                       m_yRotationPositionY,
                                       m_textColor);
}
bool DebugDisplay::UpdateZRotationText(ID3D11DeviceContext* context,
                                       const float rotZ)
{
    std::stringstream ss;

    ss << Z_ROTATION_FIELD_TEXT << ceilf(rotZ * 100) / 100 << " radians ("
       << ceilf(DirectX::XMConvertToDegrees(rotZ) * 100) / 100 << " degrees)";

    return m_zRotationText->UpdateText(context,
                                       ss.str().c_str(),
                                       m_zRotationPositionX,
                                       m_zRotationPositionY,
                                       m_textColor);
}
bool DebugDisplay::UpdateLongitudinalVelocityText(ID3D11DeviceContext* context,
                                                  const float velocityZ)
{
    std::stringstream ss;

    ss << LON_VELOCITY_FIELD_TEXT << ceilf(velocityZ * 100) / 100 << "m/s ("
       << ceilf(velocityZ * MS_TO_MPH * 100) / 100 << "mph)";

    return m_lonVelocityText->UpdateText(context,
                                         ss.str().c_str(),
                                         m_lonVelocityPositionX,
                                         m_lonVelocityPositionY,
                                         m_textColor);
}
bool DebugDisplay::UpdateLateralVelocityText(ID3D11DeviceContext* context,
                                             const float velocityX)
{
    std::stringstream ss;

    ss << LAT_VELOCITY_FIELD_TEXT << ceilf(velocityX * 100) / 100 << "m/s ("
       << ceilf(velocityX * MS_TO_MPH * 100) / 100 << "mph)";

    return m_latVelocityText->UpdateText(context,
                                         ss.str().c_str(),
                                         m_latVelocityPositionX,
                                         m_latVelocityPositionY,
                                         m_textColor);
}

bool DebugDisplay::UpdateVerticalVelocityText(ID3D11DeviceContext* context,
                                              const float velocityY)
{
    std::stringstream ss;

    ss << VERTICAL_VELOCITY_FIELD_TEXT << ceilf(velocityY * 100) / 100 << "m/s ("
       << ceilf(velocityY * MS_TO_MPH * 100) / 100 << "mph)";

    return m_verticalVelocityText->UpdateText(context,
                                              ss.str().c_str(),
                                              m_verticalVelocityPositionX,
                                              m_verticalVelocityPositionY,
                                              m_textColor);
}