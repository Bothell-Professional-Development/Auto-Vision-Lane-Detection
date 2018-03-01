#pragma once

#include "TextureShader.h"

class LightShader : public TextureShader
{
public:
    LightShader();
    LightShader(const LightShader& other);
    ~LightShader();

    virtual bool Initialize(ID3D11Device* device);
    virtual void Shutdown();

    virtual bool Initialize(ID3D11Device* device,
                            const D3D11_FILTER samplerFilteringType,
                            const unsigned int maxAnisotropy);

    bool Render(ID3D11DeviceContext* context,
                const unsigned int indexCount,
                const DirectX::XMMATRIX& world,
                const DirectX::XMMATRIX& view,
                const DirectX::XMMATRIX& projection,
                ID3D11ShaderResourceView* texture,
                const DirectX::XMFLOAT3 lightDirection,
                const DirectX::XMFLOAT3 cameraPosition,
                const DirectX::XMFLOAT4 ambientColor,
                const DirectX::XMFLOAT4 diffuseColor,
                const DirectX::XMFLOAT4 specularColor,
                const float specularPower);

private:
    struct CameraBuffer
    {
        DirectX::XMFLOAT3 cameraPosition;
        float padding;
    };

    struct LightBuffer
    {
        DirectX::XMFLOAT4 ambientColor;
        DirectX::XMFLOAT4 diffuseColor;
        DirectX::XMFLOAT4 specularColor;
        DirectX::XMFLOAT3 lightDirection;
        float specularPower;
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
                             ID3D11ShaderResourceView* texture,
                             const DirectX::XMFLOAT3 lightDirection,
                             const DirectX::XMFLOAT3 cameraPosition,
                             const DirectX::XMFLOAT4 ambientColor,
                             const DirectX::XMFLOAT4 diffuseColor,
                             const DirectX::XMFLOAT4 specularColor,
                             const float specularPower);

    ID3D11Buffer* m_cameraBuffer;
    ID3D11Buffer* m_lightBuffer;
};