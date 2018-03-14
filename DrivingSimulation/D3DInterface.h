#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <vector>
#include <d3d11.h>
#include <directxmath.h>

class D3DInterface
{
public:
    D3DInterface();
    D3DInterface(const D3DInterface& other);
    ~D3DInterface();

    //operator overloads fix alignment warning
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

    bool Initialize(const bool fullscreen,
                    const bool vSync,
                    const int screenWidth,
                    const int screenHeight,
                    const float screenDepth,
                    const float screenNear);
    void Shutdown();

    void BeginScene(const float red,
                    const float green,
                    const float blue,
                    const float alpha);
    void EndScene();

    void SetBackBufferAsRenderTarget();
    void EnableAlphaBlending();
    void DisableAlphaBlending();
    void EnableZBuffer();
    void DisableZBuffer();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();
    ID3D11DepthStencilView* GetDepthStencilView();

    const DirectX::XMMATRIX& GetProjectionMatrix();
    const DirectX::XMMATRIX& GetWorldMatrix();
    const DirectX::XMMATRIX& GetOrthoMatrix();

    const char* GetVideoHardwareInfo(unsigned int& memorySize);

private:
    bool GetDXGIAdapters(std::vector<IDXGIAdapter*>& adapters) const;

    bool GetDXGIOutputs(IDXGIAdapter* adapter,
                        std::vector<IDXGIOutput*>& outputs) const;

    bool SetAdapterDescription(IDXGIAdapter* adapter);

    bool CreateDirect3dDeviceAndSwapChain(const bool windowedMode,
                                          const unsigned int refreshNumerator,
                                          const unsigned int refreshDenominator,
                                          const unsigned int bufferCount,
                                          const unsigned int bufferWidth,
                                          const unsigned int bufferHeight,
                                          const unsigned int sampleCount,
                                          const unsigned int sampleQuality,
                                          const unsigned int flags,
                                          const DXGI_MODE_SCANLINE_ORDER scanlineOrdering,
                                          const DXGI_MODE_SCALING scalingMode,
                                          const DXGI_FORMAT bufferFormat,
                                          const DXGI_USAGE bufferUsage,
                                          const DXGI_SWAP_EFFECT swapEffect,
                                          const HWND handle);

    bool CreateDepthBuffer(const unsigned int bufferWidth,
                           const unsigned int bufferHeight,
                           const unsigned int mipLevels,
                           const unsigned int arraySize,
                           const unsigned int sampleCount,
                           const unsigned int sampleQuality,
                           const unsigned int bindFlags,
                           const unsigned int cpuAccessFlags,
                           const unsigned int miscFlags,
                           const DXGI_FORMAT format,
                           const D3D11_USAGE usage);

    bool CreateDepthStencilState(ID3D11DepthStencilState* depthStencilState,
                                 const bool depthEnable,
                                 const bool stencilEnable,
                                 const unsigned char stencilReadMask,
                                 const unsigned char stencilWriteMask,
                                 const D3D11_DEPTH_WRITE_MASK depthWriteMask,
                                 const D3D11_COMPARISON_FUNC depthFunc,
                                 const D3D11_COMPARISON_FUNC fStencilFunc,
                                 const D3D11_COMPARISON_FUNC bStencilFunc,
                                 const D3D11_STENCIL_OP fFailStencilFailOp,
                                 const D3D11_STENCIL_OP fDepthFailStencilFailOp,
                                 const D3D11_STENCIL_OP fStencilPassOp,
                                 const D3D11_STENCIL_OP bFailStencilFailOp,
                                 const D3D11_STENCIL_OP bDepthFailStencilFailOp,
                                 const D3D11_STENCIL_OP bStencilPassOp);

    bool CreateDepthStencilView(const unsigned int mipSlice,
                                const DXGI_FORMAT format,
                                const D3D11_DSV_DIMENSION viewDimension);

    bool CreateRasterState(const bool antialiasEnable,
                           const bool depthClipEnable,
                           const bool frontCCW,
                           const bool multisampleEnable,
                           const bool scissorEnable,
                           const int depthBias,
                           const float depthBiasClamp,
                           const float slopeScaledDepthBias,
                           const D3D11_FILL_MODE fillMode,
                           const D3D11_CULL_MODE cullMode);

    bool CreateAlphaBlendingStates(const D3D11_BLEND srcBlend,
                                   const D3D11_BLEND destBlend,
                                   const D3D11_BLEND_OP blendOp,
                                   const D3D11_BLEND srcBlendAlpha,
                                   const D3D11_BLEND destBlendAlpha,
                                   const D3D11_BLEND_OP blendOpAlpha,
                                   unsigned char renderTargetWriteMask);

    bool m_vSyncEnabled;
    unsigned int m_videoMemory;
    char m_videoDescription[128];

    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilState* m_depthDisabledStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    ID3D11BlendState* m_alphaEnabledBlendingState;
    ID3D11BlendState* m_alphaDisabledBlendingState;

    DirectX::XMMATRIX m_projection;
    DirectX::XMMATRIX m_world;
    DirectX::XMMATRIX m_ortho;
};