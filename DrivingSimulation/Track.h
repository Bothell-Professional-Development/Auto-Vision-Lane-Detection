#pragma once

#include "Model.h"

class Track : public Model
{
public:
    Track();
    Track(const Track& other);
    ~Track();

    bool Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context,
                    const float laneWidth,
                    const char* trackFilename,
                    const char* textureFilename,
                    const Texture::TextureImageFormat imageFormat);

    bool Initialize(ID3D11Device* device,
                    const float laneWidth,
                    const char* trackFilename,
                    DirectX::XMFLOAT4 color);

    virtual void Shutdown();

private:
    void GenerateCurve(const DirectX::XMFLOAT2 centerPoint,
                       const float radius,
                       const float startAngle,
                       const float endAngle,
                       const float angularStepAmount,
                       const bool clockwise);

    virtual bool InitializeBuffers(ID3D11Device* device);

    float m_laneWidth;
    std::string m_trackFilename;

    std::vector<DirectX::XMFLOAT2> m_centerPoints;
};
