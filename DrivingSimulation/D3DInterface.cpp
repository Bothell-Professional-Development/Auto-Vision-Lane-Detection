#include "stdafx.h"

#include "D3dInterface.h"
#include "System.h"

D3DInterface::D3DInterface() :
    m_swapChain(NULL),
    m_device(NULL),
    m_deviceContext(NULL),
    m_renderTargetView(NULL),
    m_depthStencilBuffer(NULL),
    m_depthStencilState(NULL),
    m_depthStencilView(NULL),
    m_rasterState(NULL),
    m_alphaEnabledBlendingState(NULL),
    m_alphaDisabledBlendingState(NULL){}
D3DInterface::D3DInterface(const D3DInterface& other){}
D3DInterface::~D3DInterface(){}

bool D3DInterface::Initialize(const bool fullscreen,
                              const bool vSync,
                              const int screenWidth,
                              const int screenHeight,
                              const float screenDepth,
                              const float screenNear)
{
    m_vSyncEnabled = vSync;

    std::vector<IDXGIAdapter*> adapters;
    std::vector<IDXGIOutput*> outputs;
    IDXGIAdapter* adapter = NULL;
    IDXGIOutput* adapterOutput = NULL;

    bool success = GetDXGIAdapters(adapters);

    if(success)
    {
        for(IDXGIAdapter* a : adapters)
        {
            if(GetDXGIOutputs(a, outputs))
            {
                //take first good adapter/output combination
                if(!outputs.empty())
                {
                    adapter = a;
                    adapterOutput = outputs[0];
                    break;
                }
            }
        }
        success = adapterOutput != NULL;
    }

    unsigned int numModes = 0;
    unsigned int numerator = 0;
    unsigned int denominator = 0;
    DXGI_MODE_DESC* displayModeList = NULL;

    //get refresh rate for resolution
    if(success)
    {
        //TODO: make configurable?
        if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                    DXGI_ENUM_MODES_INTERLACED,
                                                    &numModes,
                                                    NULL)))
        {
            System::GetInstance().ShowMessage(L"D3DInterface::Initialize: GetDisplayModeList call failed",
                                              L"Error");
            success = false;
        }
        else
        {
            displayModeList = new DXGI_MODE_DESC[numModes];

            if(displayModeList)
            {
                //TODO: make configurable?
                if(SUCCEEDED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                                               DXGI_ENUM_MODES_INTERLACED,
                                                               &numModes,
                                                               displayModeList)))
                {
                    for(unsigned int i = 0; i < numModes; ++i)
                    {
                        if(fullscreen)
                        {
                            if(displayModeList[i].Width == GetSystemMetrics(SM_CXSCREEN) &&
                               displayModeList[i].Height == GetSystemMetrics(SM_CYSCREEN))
                            {
                                numerator = displayModeList[i].RefreshRate.Numerator;
                                denominator = displayModeList[i].RefreshRate.Denominator;
                                break;
                            }
                        }
                        else
                        {
                            if(displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
                            {
                                numerator = displayModeList[i].RefreshRate.Numerator;
                                denominator = displayModeList[i].RefreshRate.Denominator;
                                break;
                            }
                        }
                    }

                    if(numerator == 0 || denominator == 0)
                    {
                        System::GetInstance().ShowMessage(L"D3DInterface::Initialize: Windowed resolution not supported",
                                                          L"Error");
                        success = false;
                    }
                }
            }
            else
            {
                success = false;
            }
        }
    }

    //gather video hardware information
    if(success)
    {
        success = SetAdapterDescription(adapter);
    }

    adapter = NULL;
    adapterOutput = NULL;

    if(!adapters.empty())
    {
        for(IDXGIAdapter* a : adapters)
        {
            a->Release();
        }
    }

    if(!outputs.empty())
    {
        for(IDXGIOutput* o : outputs)
        {
            o->Release();
        }
    }

    if(displayModeList)
    {
        delete[] displayModeList;
    }

    //create Direct3D device, swapchain, and render target view
    if(success)
    {
        //make configurable?
        if(CreateDirect3dDeviceAndSwapChain(!fullscreen,
                                            numerator,
                                            denominator,
                                            1, //number of buffers
                                            fullscreen ? GetSystemMetrics(SM_CXSCREEN) : screenWidth,
                                            fullscreen ? GetSystemMetrics(SM_CYSCREEN) : screenHeight,
                                            1,
                                            0,
                                            0,
                                            DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
                                            DXGI_MODE_SCALING_UNSPECIFIED,
                                            DXGI_FORMAT_R8G8B8A8_UNORM,
                                            DXGI_USAGE_RENDER_TARGET_OUTPUT,
                                            DXGI_SWAP_EFFECT_DISCARD,
                                            System::GetInstance().GetWindowHandle()))
        {
            ID3D11Texture2D* backBufferPtr;

            //assumes only one buffer exists, eventually make configurable
            if(SUCCEEDED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)))
            {
                if(FAILED(m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView)))
                {
                    System::GetInstance().ShowMessage(L"D3DInterface::Initialize: Unable to create render target view",
                                                      L"Error");
                    success = false;
                }

                backBufferPtr->Release();
                backBufferPtr = 0;
            }
            else
            {
                System::GetInstance().ShowMessage(L"D3DInterface::Initialize: Failed to acquire back buffer",
                                                  L"Error");
                success = false;
            }
        }
    }

    if(success)
    {
        if(CreateDepthBuffer(fullscreen ? GetSystemMetrics(SM_CXSCREEN) : screenWidth,
                             fullscreen ? GetSystemMetrics(SM_CYSCREEN) : screenHeight,
                             1,
                             1,
                             1,
                             0,
                             D3D11_BIND_DEPTH_STENCIL,
                             0,
                             0,
                             DXGI_FORMAT_D24_UNORM_S8_UINT,
                             D3D11_USAGE_DEFAULT))
        {
            if(CreateDepthStencilState(true,
                                       true,
                                       0xFF,
                                       0xFF,
                                       D3D11_DEPTH_WRITE_MASK_ALL,
                                       D3D11_COMPARISON_LESS,
                                       D3D11_COMPARISON_ALWAYS,
                                       D3D11_COMPARISON_ALWAYS,
                                       D3D11_STENCIL_OP_KEEP,
                                       D3D11_STENCIL_OP_INCR,
                                       D3D11_STENCIL_OP_KEEP,
                                       D3D11_STENCIL_OP_KEEP,
                                       D3D11_STENCIL_OP_INCR,
                                       D3D11_STENCIL_OP_KEEP))
            {
                success = CreateDepthStencilView(0,
                                                 DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                 D3D11_DSV_DIMENSION_TEXTURE2D);
            }
            else
            {
                success = false;
            }
        }
        else
        {
            success = false;
        }
    }

    if(success)
    {
        if(CreateRasterState(false,
                             true,
                             false,
                             false,
                             false,
                             0,
                             0.0f,
                             0.0f,
                             D3D11_FILL_SOLID,
                             D3D11_CULL_BACK))
        {
            D3D11_VIEWPORT viewport;
            viewport.Width = static_cast<float>(fullscreen ? GetSystemMetrics(SM_CXSCREEN) : screenWidth);
            viewport.Height = static_cast<float>(fullscreen ? GetSystemMetrics(SM_CYSCREEN) : screenHeight);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            viewport.TopLeftX = 0.0f;
            viewport.TopLeftY = 0.0f;

            m_deviceContext->RSSetViewports(1, &viewport);

            // Setup the projection matrix.
            float fieldOfView = DirectX::XM_PI / 4.0f;
            float screenAspect = static_cast<float>(viewport.Width) / viewport.Height;

            m_projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView,
                                                             screenAspect,
                                                             screenNear,
                                                             screenDepth);
            m_world = DirectX::XMMatrixIdentity();
            m_ortho = DirectX::XMMatrixOrthographicLH(static_cast<float>(viewport.Width),
                                                      static_cast<float>(viewport.Height),
                                                      screenNear,
                                                      screenDepth);
        }
        else
        {
            success = false;
        }
    }

    if(success)
    {
        if(!CreateAlphaBlendingStates(D3D11_BLEND_ONE,
                                      D3D11_BLEND_INV_SRC_ALPHA,
                                      D3D11_BLEND_OP_ADD,
                                      D3D11_BLEND_ONE,
                                      D3D11_BLEND_ZERO,
                                      D3D11_BLEND_OP_ADD,
                                      0x0f))
        {
            success = false;
        }
    }

    return success;
}
void D3DInterface::Shutdown()
{
    if(m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if(m_device)
    {
        m_device->Release();
        m_device = NULL;
    }

    if(m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = NULL;
    }

    if(m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = NULL;
    }

    if(m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = NULL;
    }

    if(m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = NULL;
    }

    if(m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = NULL;
    }
    
    if(m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = NULL;
    }

    if(m_alphaEnabledBlendingState)
    {
        m_alphaEnabledBlendingState->Release();
        m_alphaEnabledBlendingState = NULL;
    }

    if(m_alphaDisabledBlendingState)
    {
        m_alphaDisabledBlendingState->Release();
        m_alphaDisabledBlendingState = NULL;
    }

    if(m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = NULL;
    }
}

void D3DInterface::BeginScene(const float red,
                              const float green,
                              const float blue,
                              const float alpha)
{
    float color[4] = { red, green , blue, alpha };

    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void D3DInterface::EndScene()
{
    if(m_vSyncEnabled)
    {
        // Lock to screen refresh rate.
        m_swapChain->Present(1, 0);
    }
    else
    {
        // Present as fast as possible.
        m_swapChain->Present(0, 0);
    }
}


void D3DInterface::SetBackBufferAsRenderTarget()
{
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
}

void D3DInterface::EnableAlphaBlending()
{
    float blendFactor[4];

    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_alphaEnabledBlendingState,
                                     blendFactor,
                                     0xffffffff);
}

void D3DInterface::DisableAlphaBlending()
{
    float blendFactor[4];

    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;

    m_deviceContext->OMSetBlendState(m_alphaDisabledBlendingState,
                                     blendFactor,
                                     0xffffffff);
}

ID3D11Device* D3DInterface::GetDevice(){ return m_device; }
ID3D11DeviceContext* D3DInterface::GetDeviceContext(){ return m_deviceContext; }
ID3D11DepthStencilView* D3DInterface::GetDepthStencilView(){ return m_depthStencilView; }

const DirectX::XMMATRIX& D3DInterface::GetProjectionMatrix(){ return m_projection; }
const DirectX::XMMATRIX& D3DInterface::GetWorldMatrix(){ return m_world; }
const DirectX::XMMATRIX& D3DInterface::GetOrthoMatrix(){ return m_ortho; }


const char* D3DInterface::GetVideoHardwareInfo(unsigned int& memorySize)
{
    memorySize = m_videoMemory;

    return m_videoDescription;
}

bool D3DInterface::GetDXGIAdapters(std::vector<IDXGIAdapter*>& adapters) const
{
    bool retVal = true;
    IDXGIFactory* factory = NULL;

    if(SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
    {
        IDXGIAdapter* adapter;
        unsigned int i = 0;

        while(factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            adapters.push_back(adapter);
            ++i;
        }

        if(adapters.empty())
        {
            System::GetInstance().ShowMessage(L"D3DInterface::GetDXGIAdapters: Failed to enumerate display adapter(s)",
                                              L"Error");
            retVal = false;
        }

    }
    else
    {
        System::GetInstance().ShowMessage(L"D3DInterface::GetDXGIAdapters: CreateDXGIFactory call failed",
                                          L"Error");
        retVal = false;
    }

    if(factory)
    {
        factory->Release();
        factory = NULL;
    }

    return retVal;
}

bool D3DInterface::GetDXGIOutputs(IDXGIAdapter* adapter,
                                  std::vector<IDXGIOutput*>& outputs) const
{
    bool retVal = true;

    if(adapter)
    {
        IDXGIOutput* output;
        unsigned int i = 0;

        while(adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
        {
            outputs.push_back(output);
            ++i;
        }

        if(outputs.empty())
        {
            System::GetInstance().ShowMessage(L"D3DInterface::GetDXGIOutputs: Failed to enumerate output(s)",
                                              L"Error");
            retVal = false;
        }
    }
    else
    {
        System::GetInstance().ShowMessage(L"D3DInterface::GetDXGIOutputs: Supplied adapter is not valid",
                                          L"Error");
        retVal = false;
    }

    return retVal;
}

bool D3DInterface::SetAdapterDescription(IDXGIAdapter* adapter)
{
    bool success = true;
    DXGI_ADAPTER_DESC adapterDesc;

    if(adapter)
    {
        if(SUCCEEDED(adapter->GetDesc(&adapterDesc)))
        {
            m_videoMemory = adapterDesc.DedicatedVideoMemory / 1024 / 1024;

            size_t stringLength = 0;
            int error = wcstombs_s(&stringLength,
                                   m_videoDescription,
                                   128,
                                   adapterDesc.Description,
                                   128);

            if(error != 0)
            {
                success = false;
            }
        }
        else
        {
            success = false;
        }
    }
    else
    {
        success = false;
    }

    if(!success)
    {
        System::GetInstance().ShowMessage(L"D3DInterface::SetAdapterDescription: Unable to retrieve video hardware information",
                                          L"Error");
    }

    return success;
}

bool D3DInterface::CreateDirect3dDeviceAndSwapChain(const bool windowedMode,
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
                                                    const HWND handle)
{
    bool success = true;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.Windowed = windowedMode;

    if(m_vSyncEnabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = refreshNumerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = refreshDenominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.BufferDesc.Width = bufferWidth;
    swapChainDesc.BufferDesc.Height = bufferHeight;
    swapChainDesc.SampleDesc.Count = sampleCount;
    swapChainDesc.SampleDesc.Quality = sampleQuality;
    swapChainDesc.Flags = flags;
    swapChainDesc.BufferDesc.ScanlineOrdering = scanlineOrdering;
    swapChainDesc.BufferDesc.Scaling = scalingMode;
    swapChainDesc.BufferDesc.Format = bufferFormat;
    swapChainDesc.BufferUsage = bufferUsage;
    swapChainDesc.SwapEffect = swapEffect;
    swapChainDesc.OutputWindow = handle;

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1 };

    if(FAILED(D3D11CreateDeviceAndSwapChain(NULL,
                                            D3D_DRIVER_TYPE_HARDWARE,
                                            NULL,
                                            0,
                                            featureLevels,
                                            _countof(featureLevels),
                                            D3D11_SDK_VERSION,
                                            &swapChainDesc,
                                            &m_swapChain,
                                            &m_device,
                                            NULL,
                                            &m_deviceContext)))
    {
        System::GetInstance().ShowMessage(L"D3DInterface::CreateDirect3dDeviceAndSwapChain: Failed to create D3D Device",
                                          L"Error");
        success = false;
    }

    return success;
}

bool D3DInterface::CreateDepthBuffer(const unsigned int bufferWidth,
                                     const unsigned int bufferHeight,
                                     const unsigned int mipLevels,
                                     const unsigned int arraySize,
                                     const unsigned int sampleCount,
                                     const unsigned int sampleQuality,
                                     const unsigned int bindFlags,
                                     const unsigned int cpuAccessFlags,
                                     const unsigned int miscFlags,
                                     const DXGI_FORMAT format,
                                     const D3D11_USAGE usage)
{
    bool success = true;

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = bufferWidth;
    depthBufferDesc.Height = bufferHeight;
    depthBufferDesc.MipLevels = mipLevels;
    depthBufferDesc.ArraySize = arraySize;
    depthBufferDesc.SampleDesc.Count = sampleCount;
    depthBufferDesc.SampleDesc.Quality = sampleQuality;
    depthBufferDesc.BindFlags = bindFlags;
    depthBufferDesc.CPUAccessFlags = cpuAccessFlags;
    depthBufferDesc.MiscFlags = miscFlags;
    depthBufferDesc.Format = format;
    depthBufferDesc.Usage = usage;

    if(FAILED(m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer)))
    {
        System::GetInstance().ShowMessage(L"D3DInterface::CreateDepthBuffer: Failed to create depth stencil buffer",
                                          L"Error");
        success = false;
    }

    return success;
}

bool D3DInterface::CreateDepthStencilView(const unsigned int mipSlice,
                                          const DXGI_FORMAT format,
                                          const D3D11_DSV_DIMENSION viewDimension)
{
    bool success = true;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Texture2D.MipSlice = mipSlice;
    depthStencilViewDesc.Format = format;
    depthStencilViewDesc.ViewDimension = viewDimension;

    if(SUCCEEDED(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView)))
    {
        SetBackBufferAsRenderTarget();
    }
    else
    {
        System::GetInstance().ShowMessage(L"D3DInterface::CreateDepthStencilView: Failed to create depth stencil view",
                                          L"Error");
        success = false;
    }

    return success;
}

bool D3DInterface::CreateDepthStencilState(const bool depthEnable,
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
                                           const D3D11_STENCIL_OP bStencilPassOp)
{
    bool success = true;

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = depthEnable;
    depthStencilDesc.StencilEnable = stencilEnable;
    depthStencilDesc.StencilReadMask = stencilReadMask;
    depthStencilDesc.StencilWriteMask = stencilWriteMask;

    depthStencilDesc.DepthWriteMask = depthWriteMask;
    depthStencilDesc.DepthFunc = depthFunc;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = fFailStencilFailOp;
    depthStencilDesc.FrontFace.StencilDepthFailOp = fDepthFailStencilFailOp;
    depthStencilDesc.FrontFace.StencilPassOp = fStencilPassOp;
    depthStencilDesc.FrontFace.StencilFunc = fStencilFunc;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = bFailStencilFailOp;
    depthStencilDesc.BackFace.StencilDepthFailOp = bDepthFailStencilFailOp;
    depthStencilDesc.BackFace.StencilPassOp = bStencilPassOp;
    depthStencilDesc.BackFace.StencilFunc = bStencilFunc;

    if(SUCCEEDED(m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)))
    {
        m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
    }
    else
    {
        System::GetInstance().ShowMessage(L"D3DInterface::CreateDepthStencilState: Failed to create depth stencil state",
                                          L"Error");
        success = false;
    }

    return success;
}

bool D3DInterface::CreateRasterState(const bool antialiasEnable,
                                     const bool depthClipEnable,
                                     const bool frontCCW,
                                     const bool multisampleEnable,
                                     const bool scissorEnable,
                                     const int depthBias,
                                     const float depthBiasClamp,
                                     const float slopeScaledDepthBias,
                                     const D3D11_FILL_MODE fillMode,
                                     const D3D11_CULL_MODE cullMode)
{
    bool success = true;

    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = antialiasEnable;
    rasterDesc.DepthClipEnable = depthClipEnable;
    rasterDesc.FrontCounterClockwise = frontCCW;
    rasterDesc.MultisampleEnable = multisampleEnable;
    rasterDesc.ScissorEnable = scissorEnable;
    rasterDesc.DepthBias = depthBias;
    rasterDesc.DepthBiasClamp = depthBiasClamp;
    rasterDesc.SlopeScaledDepthBias = slopeScaledDepthBias;
    rasterDesc.FillMode = fillMode;
    rasterDesc.CullMode = cullMode;

    if(SUCCEEDED(m_device->CreateRasterizerState(&rasterDesc, &m_rasterState)))
    {
        m_deviceContext->RSSetState(m_rasterState);
    }
    else
    {
        System::GetInstance().ShowMessage(L"D3DInterface::CreateRasterState: Failed to create rasterizer state",
                                          L"Error");
        success = false;
    }

    return success;
}

bool D3DInterface::CreateAlphaBlendingStates(const D3D11_BLEND srcBlend,
                                             const D3D11_BLEND destBlend,
                                             const D3D11_BLEND_OP blendOp,
                                             const D3D11_BLEND srcBlendAlpha,
                                             const D3D11_BLEND destBlendAlpha,
                                             const D3D11_BLEND_OP blendOpAlpha,
                                             unsigned char renderTargetWriteMask)
{
    bool success = true;

    D3D11_BLEND_DESC alphaBlendDesc;

    ZeroMemory(&alphaBlendDesc, sizeof(D3D11_BLEND_DESC));

    alphaBlendDesc.RenderTarget[0].BlendEnable = true;
    alphaBlendDesc.RenderTarget[0].SrcBlend = srcBlend;
    alphaBlendDesc.RenderTarget[0].DestBlend = destBlend;
    alphaBlendDesc.RenderTarget[0].BlendOp = blendOp;
    alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = srcBlendAlpha;
    alphaBlendDesc.RenderTarget[0].DestBlendAlpha = destBlendAlpha;
    alphaBlendDesc.RenderTarget[0].BlendOpAlpha = blendOpAlpha;
    alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    if(FAILED(m_device->CreateBlendState(&alphaBlendDesc,
                                         &m_alphaEnabledBlendingState)))
    {
        success = false;
    }

    alphaBlendDesc.RenderTarget[0].BlendEnable = false;

    if(FAILED(m_device->CreateBlendState(&alphaBlendDesc,
                                         &m_alphaDisabledBlendingState)))
    {
        success = false;
    }

    if(!success)
    {
        System::GetInstance().ShowMessage(L"D3DInterface::CreateAlphaBlendingStates: Failed to create alpha blending states",
                                          L"Error");
    }

    return success;
}