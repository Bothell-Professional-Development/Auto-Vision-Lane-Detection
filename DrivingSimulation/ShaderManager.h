#pragma once

#include "ColorShader.h"
#include "LightShader.h"
#include "SkyDomeShader.h"
#include "TextShader.h"
#include "TextureShader.h"

class ShaderManager
{
public:
    ShaderManager();
    ShaderManager(const ShaderManager& other);
    ~ShaderManager();

    bool Initialize(ID3D11Device*,
                    const D3D11_FILTER samplerFilteringType,
                    const unsigned int maxAnisotropy);
    void Shutdown();

    bool RenderWithColorShader(ID3D11DeviceContext* context,
                               const unsigned int indexCount,
                               const DirectX::XMMATRIX& world,
                               const DirectX::XMMATRIX& view,
                               const DirectX::XMMATRIX& projection);
    
    bool RenderWithLightShader(ID3D11DeviceContext* context,
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

    bool RenderWithSkyDomeShader(ID3D11DeviceContext* context,
                                 const unsigned int indexCount,
                                 const DirectX::XMMATRIX& world,
                                 const DirectX::XMMATRIX& view,
                                 const DirectX::XMMATRIX& projection,
                                 const DirectX::XMFLOAT4 apexColor,
                                 const DirectX::XMFLOAT4 centerColor,
                                 const float radius);

    bool RenderWithTextShader(ID3D11DeviceContext* context,
                              const unsigned int indexCount,
                              const DirectX::XMMATRIX& world,
                              const DirectX::XMMATRIX& view,
                              const DirectX::XMMATRIX& projection,
                              ID3D11ShaderResourceView* texture,
                              const DirectX::XMFLOAT4 color);
    
    bool RenderWithTextureShader(ID3D11DeviceContext* context,
                                 const unsigned int indexCount,
                                 const DirectX::XMMATRIX& world,
                                 const DirectX::XMMATRIX& view,
                                 const DirectX::XMMATRIX& projection,
                                 ID3D11ShaderResourceView* texture);

private:
    ColorShader* m_colorShader;
    LightShader* m_lightShader;
    SkyDomeShader* m_SkyDomeShader;
    TextShader* m_textShader;
    TextureShader* m_textureShader;
};