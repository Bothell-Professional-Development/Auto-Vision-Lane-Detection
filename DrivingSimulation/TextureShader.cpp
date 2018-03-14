#include "stdafx.h"

#include "TextureShader.h"
#include "System.h"

TextureShader::TextureShader() :
    m_samplerFilteringType(D3D11_FILTER_MIN_MAG_MIP_LINEAR),
    m_maxAnisotropy(1),
    m_samplerState(NULL){}
TextureShader::TextureShader(const TextureShader& other){}
TextureShader::~TextureShader(){}

bool TextureShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device,
                            L"./Shaders/textureVertexShader.hlsl",
                            L"./Shaders/texturePixelShader.hlsl");
}

void TextureShader::Shutdown()
{
    if(m_samplerState)
    {
        m_samplerState->Release();
        m_samplerState = NULL;
    }

    Shader::Shutdown();
}    

bool TextureShader::Initialize(ID3D11Device* device,
                               const D3D11_FILTER samplerFilteringType,
                               const unsigned int maxAnisotropy)
{
    m_samplerFilteringType = samplerFilteringType;
    m_maxAnisotropy = maxAnisotropy;

    return Initialize(device);
}

bool TextureShader::Render(ID3D11DeviceContext* context,
                           const unsigned int indexCount,
                           const DirectX::XMMATRIX& world,
                           const DirectX::XMMATRIX& view,
                           const DirectX::XMMATRIX& projection,
                           ID3D11ShaderResourceView* texture)
{
    bool success = true;

    if(SetShaderParameters(context, world, view, projection, texture))
    {
        RenderShader(context, indexCount);
    }
    else
    {
        success = false;
    }

    return success;
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* context,
                                        const DirectX::XMMATRIX& world,
                                        const DirectX::XMMATRIX& view,
                                        const DirectX::XMMATRIX& projection,
                                        ID3D11ShaderResourceView* texture)
{
    bool success = true;

    const DirectX::XMMATRIX transposedWorld = DirectX::XMMatrixTranspose(world);
    const DirectX::XMMATRIX transposedView = DirectX::XMMatrixTranspose(view);
    const DirectX::XMMATRIX transposedProjection = DirectX::XMMatrixTranspose(projection);

    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if(FAILED(context->Map(m_matrixBuffer,
                           0,
                           D3D11_MAP_WRITE_DISCARD,
                           0,
                           &mappedResource)))
    {
        System::GetInstance().ShowMessage(L"TextureShader::SetShaderParameters: Failed to map matrix buffer",
                                          L"Error");
        success = false;
    }
    else
    {
        MatrixBuffer* data = static_cast<MatrixBuffer*>(mappedResource.pData);
        data->world = transposedWorld;
        data->view = transposedView;
        data->projection = transposedProjection;

        context->Unmap(m_matrixBuffer, 0);

        unsigned int bufferNumber = 0;
        context->VSSetConstantBuffers(bufferNumber,
                                      1,
                                      &m_matrixBuffer);

        context->PSSetShaderResources(0, 1, &texture);
    }

    return success;
}

bool TextureShader::InitializeShader(ID3D11Device* device,
                                     const WCHAR* vertexShaderSource,
                                     const WCHAR* pixelShaderSource)
{
    ID3D10Blob* vertexShaderBuffer = NULL;
    ID3D10Blob* pixelShaderBuffer = NULL;

    bool success = CreateShadersFromSource(device,
                                           vertexShaderSource,
                                           "TextureVertexShader",
                                           "vs_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &vertexShaderBuffer,
                                           pixelShaderSource,
                                           "TexturePixelShader",
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
            System::GetInstance().ShowMessage(L"TextureShader::InitializeShader: Failed to create input layout",
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
            System::GetInstance().ShowMessage(L"TextureShader::InitializeShader: Failed to create matrix buffer",
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
            System::GetInstance().ShowMessage(L"TextureShader::InitializeShader: Failed to create sampler state",
                                              L"Error");
            success = false;
        }
    }

    return success;
}

void TextureShader::RenderShader(ID3D11DeviceContext* context,
                                 const unsigned int indexCount)
{
    context->IASetInputLayout(m_layout);
    context->VSSetShader(m_vertexShader, NULL, 0);
    context->PSSetShader(m_pixelShader, NULL, 0);
    context->PSSetSamplers(0, 1, &m_samplerState);
    context->DrawIndexed(indexCount, 0, 0);
}