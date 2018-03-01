#include "stdafx.h"

#include "TextShader.h"
#include "System.h"

TextShader::TextShader() :
    m_pixelBuffer(NULL){}
TextShader::TextShader(const TextShader& other){}
TextShader::~TextShader(){}

bool TextShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device,
                            L"./Shaders/textVertexShader.hlsl",
                            L"./Shaders/textPixelShader.hlsl");
}

void TextShader::Shutdown()
{
    if(m_pixelBuffer)
    {
        m_pixelBuffer->Release();
        m_pixelBuffer = NULL;
    }

    TextureShader::Shutdown();
}    

bool TextShader::Initialize(ID3D11Device* device,
                            const D3D11_FILTER samplerFilteringType,
                            const unsigned int maxAnisotropy)
{
    return TextureShader::Initialize(device,
                                     samplerFilteringType,
                                     maxAnisotropy);
}

bool TextShader::Render(ID3D11DeviceContext* context,
                        const unsigned int indexCount,
                        const DirectX::XMMATRIX& world,
                        const DirectX::XMMATRIX& view,
                        const DirectX::XMMATRIX& projection,
                        ID3D11ShaderResourceView* texture,
                        const DirectX::XMFLOAT4 color)
{
    bool success = true;

    if(SetShaderParameters(context, world, view, projection, texture, color))
    {
        RenderShader(context, indexCount);
    }
    else
    {
        success = false;
    }

    return success;
}

bool TextShader::InitializeShader(ID3D11Device* device,
                                  const WCHAR* vertexShaderSource,
                                  const WCHAR* pixelShaderSource)
{
    ID3D10Blob* vertexShaderBuffer = NULL;
    ID3D10Blob* pixelShaderBuffer = NULL;

    bool success = CreateShadersFromSource(device,
                                           vertexShaderSource,
                                           "TextVertexShader",
                                           "vs_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &vertexShaderBuffer,
                                           pixelShaderSource,
                                           "TextPixelShader",
                                           "ps_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &pixelShaderBuffer);

    if(success)
    {
        D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

        polygonLayout[0].SemanticName = "POSITION";
        polygonLayout[0].SemanticIndex = 0;
        polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        polygonLayout[0].InputSlot = 0;
        polygonLayout[0].AlignedByteOffset = 0;
        polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[0].InstanceDataStepRate = 0;

        polygonLayout[1].SemanticName = "TEXCOORD";
        polygonLayout[1].SemanticIndex = 0;
        polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        polygonLayout[1].InputSlot = 0;
        polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[1].InstanceDataStepRate = 0;

        unsigned int numElements = sizeof(polygonLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

        if(FAILED(device->CreateInputLayout(polygonLayout,
                                            numElements,
                                            vertexShaderBuffer->GetBufferPointer(),
                                            vertexShaderBuffer->GetBufferSize(),
                                            &m_layout)))
        {
            System::GetInstance().ShowMessage(L"TextShader::InitializeShader: Failed to create input layout",
                                              L"Error");
            success = false;
        }
    }

    if(vertexShaderBuffer)
    {
        vertexShaderBuffer->Release();
        vertexShaderBuffer = NULL;
    }

    if(pixelShaderBuffer)
    {
        pixelShaderBuffer->Release();
        pixelShaderBuffer = NULL;
    }

    if(success)
    {
        D3D11_BUFFER_DESC matrixBufferDesc;

        matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
        matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        matrixBufferDesc.MiscFlags = 0;
        matrixBufferDesc.StructureByteStride = 0;

        if(FAILED(device->CreateBuffer(&matrixBufferDesc,
                                       NULL,
                                       &m_matrixBuffer)))
        {
            System::GetInstance().ShowMessage(L"TextShader::InitializeShader: Failed to create matrix buffer",
                                              L"Error");
            success = false;
        }
    }

    if(success)
    {
        D3D11_SAMPLER_DESC samplerDesc;

        samplerDesc.Filter = m_samplerFilteringType;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = m_maxAnisotropy;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        if(FAILED(device->CreateSamplerState(&samplerDesc,
                                             &m_samplerState)))
        {
            System::GetInstance().ShowMessage(L"TextShader::InitializeShader: Failed to create sampler state",
                                              L"Error");
            success = false;
        }
    }

    if(success)
    {
        D3D11_BUFFER_DESC pixelBufferDesc;

        pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        pixelBufferDesc.ByteWidth = sizeof(PixelBuffer);
        pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        pixelBufferDesc.MiscFlags = 0;
        pixelBufferDesc.StructureByteStride = 0;

        if(FAILED(device->CreateBuffer(&pixelBufferDesc,
                                       NULL,
                                       &m_pixelBuffer)))
        {
            System::GetInstance().ShowMessage(L"TextShader::InitializeShader: Failed to create pixel buffer",
                                              L"Error");
            success = false;
        }
    }

    return success;
}

void TextShader::RenderShader(ID3D11DeviceContext* context,
                              const unsigned int indexCount)
{
    context->IASetInputLayout(m_layout);
    context->VSSetShader(m_vertexShader, NULL, 0);
    context->PSSetShader(m_pixelShader, NULL, 0);
    context->PSSetSamplers(0, 1, &m_samplerState);
    context->DrawIndexed(indexCount, 0, 0);
}

bool TextShader::SetShaderParameters(ID3D11DeviceContext* context,
                                     const DirectX::XMMATRIX& world,
                                     const DirectX::XMMATRIX& view,
                                     const DirectX::XMMATRIX& projection,
                                     ID3D11ShaderResourceView* texture,
                                     const DirectX::XMFLOAT4 color)
{
    bool success = TextureShader::SetShaderParameters(context,
                                                      world,
                                                      view,
                                                      projection,
                                                      texture);

    if(success)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        if(FAILED(context->Map(m_pixelBuffer,
                               0,
                               D3D11_MAP_WRITE_DISCARD,
                               0,
                               &mappedResource)))
        {
            System::GetInstance().ShowMessage(L"TextShader::SetShaderParameters: Failed to map pixel buffer",
                                              L"Error");
            success = false;
        }
        else
        {
            PixelBuffer* data = static_cast<PixelBuffer*>(mappedResource.pData);
            data->pixelColor = color;

            context->Unmap(m_pixelBuffer, 0);

            unsigned int bufferNumber = 0;

            context->PSSetConstantBuffers(bufferNumber,
                                          1,
                                          &m_pixelBuffer);
        }
    }

    return success;
}