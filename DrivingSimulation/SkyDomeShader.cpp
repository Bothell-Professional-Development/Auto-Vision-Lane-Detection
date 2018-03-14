#include "stdafx.h"

#include "SkyDomeShader.h"
#include "System.h"

SkyDomeShader::SkyDomeShader() :
    m_gradientBuffer(NULL){}
SkyDomeShader::SkyDomeShader(const SkyDomeShader& other){}
SkyDomeShader::~SkyDomeShader(){}

bool SkyDomeShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device,
                            L"./Shaders/SkyDomeVertexShader.hlsl",
                            L"./Shaders/SkyDomePixelShader.hlsl");
}

void SkyDomeShader::Shutdown()
{
    if(m_gradientBuffer)
    {
        m_gradientBuffer->Release();
        m_gradientBuffer = NULL;
    }

    Shader::Shutdown();
}

bool SkyDomeShader::Render(ID3D11DeviceContext* context,
                           const unsigned int indexCount,
                           const DirectX::XMMATRIX& world,
                           const DirectX::XMMATRIX& view,
                           const DirectX::XMMATRIX& projection,
                           const DirectX::XMFLOAT4 apexColor,
                           const DirectX::XMFLOAT4 centerColor,
                           const float radius)
{
    bool success = true;

    if(SetShaderParameters(context,
                           world,
                           view,
                           projection,
                           apexColor,
                           centerColor,
                           radius))
    {
        RenderShader(context, indexCount);
    }
    else
    {
        success = false;
    }

    return success;
}

bool SkyDomeShader::InitializeShader(ID3D11Device* device,
                                     const WCHAR* vertexShaderSource,
                                     const WCHAR* pixelShaderSource)
{
    ID3D10Blob* vertexShaderBuffer = NULL;
    ID3D10Blob* pixelShaderBuffer = NULL;

    bool success = CreateShadersFromSource(device,
                                           vertexShaderSource,
                                           "SkyDomeVertexShader",
                                           "vs_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &vertexShaderBuffer,
                                           pixelShaderSource,
                                           "SkyDomePixelShader",
                                           "ps_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &pixelShaderBuffer);

    if(success)
    {
        D3D11_INPUT_ELEMENT_DESC polygonLayout[1];

        polygonLayout[0].SemanticName = "POSITION";
        polygonLayout[0].SemanticIndex = 0;
        polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        polygonLayout[0].InputSlot = 0;
        polygonLayout[0].AlignedByteOffset = 0;
        polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        polygonLayout[0].InstanceDataStepRate = 0;

        unsigned int numElements = sizeof(polygonLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

        if(FAILED(device->CreateInputLayout(polygonLayout,
                                            numElements,
                                            vertexShaderBuffer->GetBufferPointer(),
                                            vertexShaderBuffer->GetBufferSize(),
                                            &m_layout)))
        {
            System::GetInstance().ShowMessage(L"SkyDomeShader::InitializeShader: Failed to create input layout",
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
            System::GetInstance().ShowMessage(L"SkyDomeShader::InitializeShader: Failed to create matrix buffer",
                                              L"Error");
            success = false;
        }
    }

    if(success)
    {
        D3D11_BUFFER_DESC gradientBufferDesc;
        gradientBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        gradientBufferDesc.ByteWidth = sizeof(GradientBuffer);
        gradientBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        gradientBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        gradientBufferDesc.MiscFlags = 0;
        gradientBufferDesc.StructureByteStride = 0;

        if(FAILED(device->CreateBuffer(&gradientBufferDesc,
                                       NULL,
                                       &m_gradientBuffer)))
        {
            System::GetInstance().ShowMessage(L"SkyDomeShader::InitializeShader: Failed to create gradient buffer",
                                              L"Error");
            success = false;
        }
    }

    return success;
}

void SkyDomeShader::RenderShader(ID3D11DeviceContext* context,
                                 const unsigned int indexCount)
{
    context->IASetInputLayout(m_layout);
    context->VSSetShader(m_vertexShader, NULL, 0);
    context->PSSetShader(m_pixelShader, NULL, 0);
    context->DrawIndexed(indexCount, 0, 0);
}

bool SkyDomeShader::SetShaderParameters(ID3D11DeviceContext* context,
                                        const DirectX::XMMATRIX& world,
                                        const DirectX::XMMATRIX& view,
                                        const DirectX::XMMATRIX& projection,
                                        const DirectX::XMFLOAT4 apexColor,
                                        const DirectX::XMFLOAT4 centerColor,
                                        const float radius)
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
        System::GetInstance().ShowMessage(L"SkyDomeShader::SetShaderParameters: Failed to map matrix buffer",
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
    }

    if(success)
    {
        if(FAILED(context->Map(m_gradientBuffer,
                               0,
                               D3D11_MAP_WRITE_DISCARD,
                               0,
                               &mappedResource)))
        {
            System::GetInstance().ShowMessage(L"SkyDomeShader::SetShaderParameters: Failed to map gradient buffer",
                                              L"Error");
            success = false;
        }
        else
        {
            GradientBuffer* data = static_cast<GradientBuffer*>(mappedResource.pData);
            data->apexColor = apexColor;
            data->centerColor = centerColor;
            data->radius = radius;

            context->Unmap(m_gradientBuffer, 0);

            unsigned int bufferNumber = 0;
            context->PSSetConstantBuffers(bufferNumber,
                                          1,
                                          &m_gradientBuffer);
        }
    }

    return success;
}