#pragma once

#include "D3DInterface.h"
#include "MovementControls.h"
#include "PerformanceMetrics.h"
#include "Position.h"
#include "ShaderManager.h"
#include "Text.h"

//Concept Layout:
//Display adapter details:
//Screen resolution and refresh rate:

//Frame Rate:
//CPU Util:

//Position X:
//Position Y:
//Position Z

//Rotation X:
//Rotation Y:
//Rotation Z

//Longitudinal Velocity:
//Lateral Velocity:
//Vertical Velocity:

class DebugDisplay
{
public:
    DebugDisplay();
    DebugDisplay(const DebugDisplay& other);
    ~DebugDisplay();

    bool Initialize(D3DInterface* d3dInterface,
                    const unsigned int positionX,
                    const unsigned int positionY,
                    const unsigned int textHeight,
                    const unsigned int verticalSpacing,
                    const DirectX::XMFLOAT4 textColor,
                    const float updatesPerSecond,
                    const unsigned int screenWidth,
                    const unsigned int screenHeight,
                    const char* textureFilename,
                    const Texture::TextureImageFormat imageFormat);

    void Shutdown();
    bool Frame(ID3D11DeviceContext* context,
               const Position& position,
               const MovementControls& movementControls);

    bool Render(ID3D11DeviceContext* context,
                ShaderManager* shaderManager,
                const DirectX::XMMATRIX& world,
                const DirectX::XMMATRIX& view,
                const DirectX::XMMATRIX& projection);

private:
    bool UpdateAdapterDescriptionText(ID3D11DeviceContext* context,
                                      const char* adapterDescription);

    bool UpdateAdapterMemoryText(ID3D11DeviceContext* context,
                                 const unsigned int videoMemory);

    bool UpdateScreenResolutionText(ID3D11DeviceContext* context,
                                    const unsigned int width,
                                    const unsigned int height);

    bool UpdateFrameRateText(ID3D11DeviceContext* context);

    bool UpdateProcessorUtilizationText(ID3D11DeviceContext* context);

    bool UpdateXPositionText(ID3D11DeviceContext* context,
                             const float posY);

    bool UpdateYPositionText(ID3D11DeviceContext* context,
                             const float posZ);

    bool UpdateZPositionText(ID3D11DeviceContext* context,
                             const float posX);

    bool UpdateXRotationText(ID3D11DeviceContext* context,
                             const float rotX);

    bool UpdateYRotationText(ID3D11DeviceContext* context,
                             const float rotY);

    bool UpdateZRotationText(ID3D11DeviceContext* context,
                             const float rotZ);

    bool UpdateLongitudinalVelocityText(ID3D11DeviceContext* context,
                                        const float velocity);

    bool UpdateLateralVelocityText(ID3D11DeviceContext* context,
                                   const float velocity);

    bool UpdateVerticalVelocityText(ID3D11DeviceContext* context,
                                    const float velocity);

    static const std::string ADAPTER_DESCRIPTION_FIELD_TEXT;
    static const std::string ADAPTER_MEMORY_FIELD_TEXT;
    static const std::string SCREEN_RESOLUTION_FIELD_TEXT;
    static const std::string FRAME_RATE_FIELD_TEXT;
    static const std::string CPU_UTILIZATION_FIELD_TEXT;
    static const std::string X_POSITION_FIELD_TEXT;
    static const std::string Y_POSITION_FIELD_TEXT;
    static const std::string Z_POSITION_FIELD_TEXT;
    static const std::string X_ROTATION_FIELD_TEXT;
    static const std::string Y_ROTATION_FIELD_TEXT;
    static const std::string Z_ROTATION_FIELD_TEXT;
    static const std::string LON_VELOCITY_FIELD_TEXT;
    static const std::string LAT_VELOCITY_FIELD_TEXT;
    static const std::string VERTICAL_VELOCITY_FIELD_TEXT;

    static const unsigned char ADAPTER_DESCRIPTION_VALUE_LENGTH;
    static const unsigned char ADAPTER_MEMORY_VALUE_LENGTH;
    static const unsigned char SCREEN_RESOLUTION_VALUE_LENGTH;
    static const unsigned char FRAME_RATE_VALUE_LENGTH;
    static const unsigned char CPU_UTILIZATION_VALUE_LENGTH;
    static const unsigned char POSITION_VALUE_LENGTH;
    static const unsigned char ROTATION_VALUE_LENGTH;
    static const unsigned char VELOCITY_VALUE_LENGTH;

    static const float MS_TO_MPH;

    unsigned int m_adapterDescriptionPositionX;
    unsigned int m_adapterDescriptionPositionY;
    unsigned int m_adapterMemoryPositionX;
    unsigned int m_adapterMemoryPositionY;
    unsigned int m_screenResolutionPositionX;
    unsigned int m_screenResolutionPositionY;
    unsigned int m_frameRatePositionX;
    unsigned int m_frameRatePositionY;
    unsigned int m_processorUtilizationPositionX;
    unsigned int m_processorUtilizationPositionY;
    
    unsigned int m_xPositionX;
    unsigned int m_xPositionY;
    unsigned int m_yPositionX;
    unsigned int m_yPositionY;
    unsigned int m_zPositionX;
    unsigned int m_zPositionY;

    unsigned int m_xRotationPositionX;
    unsigned int m_xRotationPositionY;
    unsigned int m_yRotationPositionX;
    unsigned int m_yRotationPositionY;
    unsigned int m_zRotationPositionX;
    unsigned int m_zRotationPositionY;

    unsigned int m_lonVelocityPositionX;
    unsigned int m_lonVelocityPositionY;
    unsigned int m_latVelocityPositionX;
    unsigned int m_latVelocityPositionY;
    unsigned int m_verticalVelocityPositionX;
    unsigned int m_verticalVelocityPositionY;

    DirectX::XMFLOAT4 m_textColor;

    PerformanceMetrics* m_performanceMetrics;

    Text* m_adapterDescriptionText;
    Text* m_adapterMemoryText;
    Text* m_screenResolutionText;
    Text* m_frameRateText;
    Text* m_processorUtilizationText;
    Text* m_xPositionText;
    Text* m_yPositionText;
    Text* m_zPositionText;
    Text* m_xRotationText;
    Text* m_yRotationText;
    Text* m_zRotationText;
    Text* m_lonVelocityText;
    Text* m_latVelocityText;
    Text* m_verticalVelocityText;

    std::vector<Text*> m_textList;
};