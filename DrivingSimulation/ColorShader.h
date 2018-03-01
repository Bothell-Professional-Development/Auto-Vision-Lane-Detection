#pragma once

#include "Shader.h"

class ColorShader : public Shader
{
public:
    ColorShader();
    ColorShader(const ColorShader& other);
    ~ColorShader();

    virtual bool Initialize(ID3D11Device* device);
    virtual void Shutdown();

    bool Render(ID3D11DeviceContext* context,
                const unsigned int indexCount,
                const DirectX::XMMATRIX& world,
                const DirectX::XMMATRIX& view,
                const DirectX::XMMATRIX& projection);

private:
    virtual bool InitializeShader(ID3D11Device* device,
                                  const WCHAR* vertexShaderSource,
                                  const WCHAR* pixelShaderSource);

    virtual void RenderShader(ID3D11DeviceContext* context,
                              const unsigned int indexCount);

    bool SetShaderParameters(ID3D11DeviceContext* context,
                             const DirectX::XMMATRIX& world,
                             const DirectX::XMMATRIX& view,
                             const DirectX::XMMATRIX& projection);
};