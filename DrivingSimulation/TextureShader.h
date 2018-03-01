#pragma once

#include "Shader.h"

class TextureShader : public Shader
{
public:
    TextureShader();
    TextureShader(const TextureShader& other);
    ~TextureShader();

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
                ID3D11ShaderResourceView* texture);

protected:
    bool SetShaderParameters(ID3D11DeviceContext* context,
                             const DirectX::XMMATRIX& world,
                             const DirectX::XMMATRIX& view,
                             const DirectX::XMMATRIX& projection,
                             ID3D11ShaderResourceView* texture);

    D3D11_FILTER m_samplerFilteringType;
    unsigned int m_maxAnisotropy;
    ID3D11SamplerState* m_samplerState;

private:
    virtual bool InitializeShader(ID3D11Device* device,
                                  const WCHAR* vertexShaderSource,
                                  const WCHAR* pixelShaderSource);

    virtual void RenderShader(ID3D11DeviceContext* context,
                              const unsigned int indexCount);
};