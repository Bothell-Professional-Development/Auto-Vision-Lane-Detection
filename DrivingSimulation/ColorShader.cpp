#include "stdafx.h"

#include "ColorShader.h"
#include "System.h"

ColorShader::ColorShader(){}
ColorShader::ColorShader(const ColorShader& other){}
ColorShader::~ColorShader(){}

bool ColorShader::Initialize(ID3D11Device* device)
{
    return InitializeShader(device,
                            L"./Shaders/colorVertexShader.hlsl",
                            L"./Shaders/colorPixelShader.hlsl");
}

void ColorShader::Shutdown()
{
    Shader::Shutdown();
}

bool ColorShader::Render(ID3D11DeviceContext* context,
                         const unsigned int indexCount,
                         const DirectX::XMMATRIX& world,
                         const DirectX::XMMATRIX& view,
                         const DirectX::XMMATRIX& projection)
{
    bool success = true;

    if(SetShaderParameters(context, world, view, projection))
    {
        RenderShader(context, indexCount);
    }
    else
    {
        success = false;
    }

    return success;
}

bool ColorShader::InitializeShader(ID3D11Device* device,
                                   const WCHAR* vertexShaderSource,
                                   const WCHAR* pixelShaderSource)
{
    ID3D10Blob* vertexShaderBuffer = NULL;
    ID3D10Blob* pixelShaderBuffer = NULL;

    bool success = CreateShadersFromSource(device,
                                           vertexShaderSource,
                                           "ColorVertexShader",
                                           "vs_5_0",
                                           D3D10_SHADER_ENABLE_STRICTNESS,
                                           0,
                                           &vertexShaderBuffer,
                                           pixelShaderSource,
                                           "ColorPixelShader",
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

        polygonLayout[1].SemanticName = "COLOR";
        polygonLayout[1].SemanticIndex = 0;
        polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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
            System::GetInstance().ShowMessage(L"ColorShader::InitializeShader: Failed to create input layout",
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
            System::GetInstance().ShowMessage(L"ColorShader::InitializeShader: Failed to create matrix buffer",
                                              L"Error");
            success = false;
        }
    }

    return success;
}

void ColorShader::RenderShader(ID3D11DeviceContext* context,
                               const unsigned int indexCount)
{
    context->IASetInputLayout(m_layout);
    context->VSSetShader(m_vertexShader, NULL, 0);
    context->PSSetShader(m_pixelShader, NULL, 0);
    context->DrawIndexed(indexCount, 0, 0);
}

bool ColorShader::SetShaderParameters(ID3D11DeviceContext* context,
                                      const DirectX::XMMATRIX& world,
                                      const DirectX::XMMATRIX& view,
                                      const DirectX::XMMATRIX& projection)
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
        System::GetInstance().ShowMessage(L"ColorShader::SetShaderParameters: Failed to map matrix buffer",
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

    return success;
}
