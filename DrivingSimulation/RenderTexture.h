#pragma once

#include <DirectXMath.h>

#include "Texture.h"

class RenderTexture : public Texture
{
public:
    RenderTexture();
    RenderTexture(const Texture& other);
    ~RenderTexture();

    bool Initialize(ID3D11Device* device,
                    const unsigned int width,
                    const unsigned int height);

    void SetAsRenderTarget(ID3D11DeviceContext* context,
                           ID3D11DepthStencilView* depthStencilView);

    void ClearTextureData(ID3D11DeviceContext* context,
                          ID3D11DepthStencilView* depthStencilView,
                          const DirectX::XMFLOAT4 backgroundColor);

    virtual void Shutdown();

private:
    ID3D11RenderTargetView* m_renderTargetView;
};