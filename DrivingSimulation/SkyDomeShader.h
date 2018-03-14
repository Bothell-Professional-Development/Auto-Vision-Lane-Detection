#pragma once

#include "Shader.h"

class SkyDomeShader : public Shader
{
public:
    SkyDomeShader();
    SkyDomeShader(const SkyDomeShader& other);
    ~SkyDomeShader();

    virtual bool Initialize(ID3D11Device* device);
    virtual void Shutdown();

    bool Render(ID3D11DeviceContext* context,
                const unsigned int indexCount,
                const DirectX::XMMATRIX& world,
                const DirectX::XMMATRIX& view,
                const DirectX::XMMATRIX& projection,
                const DirectX::XMFLOAT4 apexColor,
                const DirectX::XMFLOAT4 centerColor,
                const float radius);
private:
    struct GradientBuffer
    {
        DirectX::XMFLOAT4 apexColor;
        DirectX::XMFLOAT4 centerColor;
        float radius;
        DirectX::XMFLOAT3 padding;
    };

    virtual bool InitializeShader(ID3D11Device* device,
                                  const WCHAR* vertexShaderSource,
                                  const WCHAR* pixelShaderSource);

    virtual void RenderShader(ID3D11DeviceContext* context,
                              const unsigned int indexCount);

    bool SetShaderParameters(ID3D11DeviceContext* context,
                             const DirectX::XMMATRIX& world,
                             const DirectX::XMMATRIX& view,
                             const DirectX::XMMATRIX& projection,
                             const DirectX::XMFLOAT4 apexColor,
                             const DirectX::XMFLOAT4 centerColor,
                             const float radius);

    ID3D11Buffer* m_gradientBuffer;
};