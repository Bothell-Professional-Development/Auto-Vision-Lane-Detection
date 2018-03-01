#pragma once

#include "TextureShader.h"

class TextShader : public TextureShader
{
public:
    TextShader();
    TextShader(const TextShader& other);
    ~TextShader();

    virtual bool Initialize(ID3D11Device* device);
    virtual void Shutdown();

    bool Initialize(ID3D11Device* device,
                    const D3D11_FILTER samplerFilteringType,
                    const unsigned int maxAnisotropy);

    bool Render(ID3D11DeviceContext* context,
                const unsigned int indexCount,
                const DirectX::XMMATRIX& world,
                const DirectX::XMMATRIX& view,
                const DirectX::XMMATRIX& projection,
                ID3D11ShaderResourceView* texture,
                const DirectX::XMFLOAT4 color);

private:
    struct PixelBuffer
    {
        DirectX::XMFLOAT4 pixelColor;
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
                             const DirectX::XMFLOAT4 color);

    ID3D11Buffer* m_pixelBuffer;
};