#include "stdafx.h"

#include "ShaderManager.h"

ShaderManager::ShaderManager() :
    m_colorShader(NULL),
    m_lightShader(NULL),
    m_textShader(NULL),
    m_textureShader(NULL){}
ShaderManager::ShaderManager(const ShaderManager& other){}
ShaderManager::~ShaderManager(){}

bool ShaderManager::Initialize(ID3D11Device* device,
                               const D3D11_FILTER samplerFilteringType,
                               const unsigned int maxAnisotropy)
{
    m_colorShader = new ColorShader();
    bool success = m_colorShader->Initialize(device);
    
    if(success)
    {
        m_lightShader = new LightShader();
        success = static_cast<TextureShader*>(m_lightShader)->Initialize(device,
                                                                         samplerFilteringType,
                                                                         maxAnisotropy);
    }

    if(success)
    {
        m_textShader = new TextShader();
        success = static_cast<TextureShader*>(m_textShader)->Initialize(device,
                                                                        samplerFilteringType,
                                                                        maxAnisotropy);
    }

    if(success)
    {
        m_textureShader = new TextureShader();
        success = m_textureShader->Initialize(device,
                                              samplerFilteringType,
                                              maxAnisotropy);
    }

    return success;
}

void ShaderManager::Shutdown()
{
    if(m_colorShader)
    {
        m_colorShader->Shutdown();
        delete m_colorShader;
        m_colorShader = NULL;
    }

    if(m_lightShader)
    {
        m_lightShader->Shutdown();
        delete m_lightShader;
        m_lightShader = NULL;
    }

    if(m_textShader)
    {
        m_textShader->Shutdown();
        delete m_textShader;
        m_textShader = NULL;
    }

    if(m_textureShader)
    {
        m_textureShader->Shutdown();
        delete m_textureShader;
        m_textureShader = NULL;
    }
}

bool ShaderManager::RenderWithColorShader(ID3D11DeviceContext* context,
                                          const unsigned int indexCount,
                                          const DirectX::XMMATRIX& world,
                                          const DirectX::XMMATRIX& view,
                                          const DirectX::XMMATRIX& projection)
{
    return m_colorShader->Render(context,
                                 indexCount,
                                 world,
                                 view,
                                 projection);
}

bool ShaderManager::RenderWithLightShader(ID3D11DeviceContext* context,
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
                                          const float specularPower)
{
    return m_lightShader->Render(context,
                                 indexCount,
                                 world,
                                 view,
                                 projection,
                                 texture,
                                 lightDirection,
                                 cameraPosition,
                                 ambientColor,
                                 diffuseColor,
                                 specularColor,
                                 specularPower);
}

bool ShaderManager::RenderWithTextShader(ID3D11DeviceContext* context,
                                         const unsigned int indexCount,
                                         const DirectX::XMMATRIX& world,
                                         const DirectX::XMMATRIX& view,
                                         const DirectX::XMMATRIX& projection,
                                         ID3D11ShaderResourceView* texture,
                                         const DirectX::XMFLOAT4 color)
{
    return m_textShader->Render(context,
                                indexCount,
                                world,
                                view,
                                projection,
                                texture,
                                color);
}

bool ShaderManager::RenderWithTextureShader(ID3D11DeviceContext* context,
                                            const unsigned int indexCount,
                                            const DirectX::XMMATRIX& world,
                                            const DirectX::XMMATRIX& view,
                                            const DirectX::XMMATRIX& projection,
                                            ID3D11ShaderResourceView* texture)
{
    return m_textureShader->Render(context,
                                   indexCount,
                                   world,
                                   view,
                                   projection,
                                   texture);
}