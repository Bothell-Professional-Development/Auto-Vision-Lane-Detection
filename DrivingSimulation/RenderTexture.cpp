#include "stdafx.h"

#include "RenderTexture.h"
#include "System.h"

RenderTexture::RenderTexture() :
    m_renderTargetView(NULL){}
RenderTexture::RenderTexture(const Texture& other){}
RenderTexture::~RenderTexture(){}

bool RenderTexture::Initialize(ID3D11Device* device,
                               const unsigned int width,
                               const unsigned int height)
{
    bool success = true;

    m_width = width;
    m_height = height;

    m_raster = new unsigned char[width * height * 4];

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    textureDesc.Width = m_width;
    textureDesc.Height = m_height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    if(FAILED(device->CreateTexture2D(&textureDesc,
                                      NULL,
                                      &m_texture)))
    {
        System::GetInstance().ShowMessage(L"RenderTexture::Initialize: Failed to create texture",
                                          L"Error");
        success = false;
    }

    if(success)
    {
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        if(FAILED(device->CreateRenderTargetView(m_texture,
                                                 &renderTargetViewDesc,
                                                 &m_renderTargetView)))
        {
            System::GetInstance().ShowMessage(L"RenderTexture::Initialize: Failed to create render target view",
                                              L"Error");
            success = false;
        }
    }

    if(success)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        if(FAILED(device->CreateShaderResourceView(m_texture, &shaderResourceViewDesc, &m_textureView)))
        {
            System::GetInstance().ShowMessage(L"RenderTexture::Initialize: Failed to create shader resource view",
                                              L"Error");
            success = false;
        }
    }

    return success;
}

void RenderTexture::SetAsRenderTarget(ID3D11DeviceContext* context,
                                      ID3D11DepthStencilView* depthStencilView)
{
    if(m_renderTargetView)
    {
        context->OMSetRenderTargets(1,
                                    &m_renderTargetView,
                                    depthStencilView);
    }
}

void RenderTexture::ClearTextureData(ID3D11DeviceContext* context,
                                     ID3D11DepthStencilView* depthStencilView,
                                     const DirectX::XMFLOAT4 backgroundColor)
{
    if(m_renderTargetView)
    {
        float color[4] = { backgroundColor.x,
                           backgroundColor.y,
                           backgroundColor.z,
                           backgroundColor.w, };

        context->ClearRenderTargetView(m_renderTargetView, color);
        context->ClearDepthStencilView(depthStencilView,
                                       D3D11_CLEAR_DEPTH,
                                       1.0f,
                                       0);
    }
}

void RenderTexture::Shutdown()
{
    Texture::Shutdown();

    if(m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = NULL;
    }
}