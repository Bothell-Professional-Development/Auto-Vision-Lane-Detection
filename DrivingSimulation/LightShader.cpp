#include "stdafx.h"

#include "LightShader.h"
#include "System.h"

LightShader::LightShader() :
    m_cameraBuffer(NULL),
    m_lightBuffer(NULL){}
LightShader::LightShader(const LightShader& other){}
LightShader::~LightShader(){}

bool LightShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device,
                            L"./Shaders/lightVertexShader.hlsl",
                            L"./Shaders/lightPixelShader.hlsl");
}   

bool LightShader::Initialize(ID3D11Device* device,
                             const D3D11_FILTER samplerFilteringType,
                             const unsigned int maxAnisotropy)
{
    return TextureShader::Initialize(device,
                                     samplerFilteringType,
                                     maxAnisotropy);
}

void LightShader::Shutdown()
{
    if(m_cameraBuffer)
    {
        m_cameraBuffer->Release();
        m_cameraBuffer = NULL;
    }

    if(m_lightBuffer)
    {
        m_lightBuffer->Release();
        m_lightBuffer = NULL;
    }

    TextureShader::Shutdown();
}

//bool LightShader::Initialize(ID3D11Device* device,
//                             const D3D11_FILTER samplerFilteringType,
//                             const unsigned int maxAnisotropy)
//{
//    m_samplerFilteringType = samplerFilteringType;
//    m_maxAnisotropy = maxAnisotropy;
//
//    return Initialize(device);
//}

bool LightShader::Render(ID3D11DeviceContext* context,
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
    bool success = true;

    if(SetShaderParameters(context,
                           world,
                           view,
                           projection,
                           texture,
                           lightDirection,
                           cameraPosition,
                           ambientColor,
                           diffuseColor,
                           specularColor,
                           specularPower))
    {
        RenderShader(context, indexCount);
    }
    else
    {
        success = false;
    }

    return success;
}

bool LightShader::InitializeShader(ID3D11Device* device,
                                   const WCHAR* vertexShaderSource,
                                   const WCHAR* pixelShaderSource)
{
    ID3D10Blob* vertexShaderBuffer = NULL;
    ID3D10Blob* pixelShaderBuffer = NULL;

    bool success = CreateShadersFromSource(device,
                                           vertexShaderSource,
                                           "LightVertexShader",
                                           "vs_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &vertexShaderBuffer,
                                           pixelShaderSource,
                                           "LightPixelShader",
                                           "ps_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &pixelShaderBuffer);

    if(success)
    {
        D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

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

        polygonLayout[2].SemanticName = "NORMAL";
        polygonLayout[2].SemanticIndex = 0;
        polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        polygonLayout[2].InputSlot = 0;
        polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[2].InstanceDataStepRate = 0;

        unsigned int numElements = sizeof(polygonLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

        if(FAILED(device->CreateInputLayout(polygonLayout,
                                            numElements,
                                            vertexShaderBuffer->GetBufferPointer(),
                                            vertexShaderBuffer->GetBufferSize(),
                                            &m_layout)))
        {
            System::GetInstance().ShowMessage(L"LightShader::InitializeShader: Failed to create input layout",
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
            System::GetInstance().ShowMessage(L"LightShader::InitializeShader: Failed to create matrix buffer",
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
            System::GetInstance().ShowMessage(L"LightShader::InitializeShader: Failed to create sampler state",
                                              L"Error");
            success = false;
        }
    }

    if(success)
    {
        D3D11_BUFFER_DESC cameraBufferDesc;

        cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        cameraBufferDesc.ByteWidth = sizeof(CameraBuffer);
        cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cameraBufferDesc.MiscFlags = 0;
        cameraBufferDesc.StructureByteStride = 0;

        if(FAILED(device->CreateBuffer(&cameraBufferDesc,
                                       NULL,
                                       &m_cameraBuffer)))
        {
            System::GetInstance().ShowMessage(L"LightShader::InitializeShader: Failed to create camera buffer",
                                              L"Error");
            success = false;
        }
    }

    if(success)
    {
        D3D11_BUFFER_DESC lightBufferDesc;

        lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        lightBufferDesc.ByteWidth = sizeof(LightBuffer);
        lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        lightBufferDesc.MiscFlags = 0;
        lightBufferDesc.StructureByteStride = 0;

        if(FAILED(device->CreateBuffer(&lightBufferDesc,
                                       NULL,
                                       &m_lightBuffer)))
        {
            System::GetInstance().ShowMessage(L"LightShader::InitializeShader: Failed to create light buffer",
                                              L"Error");
            success = false;
        }
    }

    return success;
}

void LightShader::RenderShader(ID3D11DeviceContext* context,
                               const unsigned int indexCount)
{
    context->IASetInputLayout(m_layout);
    context->VSSetShader(m_vertexShader, NULL, 0);
    context->PSSetShader(m_pixelShader, NULL, 0);
    context->PSSetSamplers(0, 1, &m_samplerState);
    context->DrawIndexed(indexCount, 0, 0);
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* context,
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
    bool success = TextureShader::SetShaderParameters(context,
                                                      world,
                                                      view,
                                                      projection,
                                                      texture);

    if(success)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        if(FAILED(context->Map(m_cameraBuffer,
                               0,
                               D3D11_MAP_WRITE_DISCARD,
                               0,
                               &mappedResource)))
        {
            System::GetInstance().ShowMessage(L"LightShader::SetShaderParameters: Failed to map camera buffer",
                                              L"Error");
            success = false;
        }
        else
        {
            CameraBuffer* data = static_cast<CameraBuffer*>(mappedResource.pData);
            data->cameraPosition = cameraPosition;
            data->padding = 0.0f;

            context->Unmap(m_cameraBuffer, 0);

            unsigned int bufferNumber = 1;
            context->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
        }
    }

    if(success)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        if(FAILED(context->Map(m_lightBuffer,
                               0,
                               D3D11_MAP_WRITE_DISCARD,
                               0,
                               &mappedResource)))
        {
            System::GetInstance().ShowMessage(L"LightShader::SetShaderParameters: Failed to map light buffer",
                                              L"Error");
            success = false;
        }
        else
        {
            LightBuffer* data = static_cast<LightBuffer*>(mappedResource.pData);
            data->lightDirection = lightDirection;
            data->ambientColor = ambientColor;
            data->diffuseColor = diffuseColor;
            data->specularColor = specularColor;
            data->specularPower = specularPower;

            context->Unmap(m_lightBuffer, 0);

            unsigned int bufferNumber = 0;
            context->PSSetConstantBuffers(bufferNumber,
                                          1,
                                          &m_lightBuffer);
        }
    }

    return success;
}