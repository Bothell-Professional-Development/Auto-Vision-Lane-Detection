#include "stdafx.h"

#include "Shader.h"
#include "System.h"

const std::string Shader::SHADER_ERROR_LOG_FILE_NAME = "./shader_errors.txt";

Shader::Shader() :
    m_vertexShader(NULL),
    m_pixelShader(NULL),
    m_layout(NULL),
    m_matrixBuffer(NULL){}
Shader::Shader(const Shader& other){}
Shader::~Shader(){}

void Shader::Shutdown()
{
    if(m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = NULL;
    }

    if(m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = NULL;
    }

    if(m_layout)
    {
        m_layout->Release();
        m_layout = NULL;
    }

    if(m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = NULL;
    }
}

bool Shader::CreateShadersFromSource(ID3D11Device* device,
                                     const WCHAR* vertexShaderSource,
                                     const LPCSTR vertexShaderEntryPoint,
                                     const LPCSTR vertexShaderTarget,
                                     const unsigned int vertexShaderFlags1,
                                     const unsigned int vertexShaderFlags2,
                                     ID3D10Blob** vertexShaderBuffer,
                                     const WCHAR* pixelShaderSource,
                                     const LPCSTR pixelShaderEntryPoint,
                                     const LPCSTR pixelShaderTarget,
                                     const unsigned int pixelShaderFlags1,
                                     const unsigned int pixelShaderFlags2,
                                     ID3D10Blob** pixelShaderBuffer)
{
    bool success = true;

    ID3D10Blob* errorMessage = NULL;

    if(FAILED(D3DCompileFromFile(vertexShaderSource,
                                 NULL,
                                 NULL,
                                 vertexShaderEntryPoint,
                                 vertexShaderTarget,
                                 vertexShaderFlags1,
                                 vertexShaderFlags2,
                                 vertexShaderBuffer,
                                 &errorMessage)))
    {
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage);
        }
        else
        {
            System::GetInstance().ShowMessage(L"Shader::CreateShadersFromSource: Unable to locate vertex shader source",
                                              L"Error");
        }

        success = false;
    }
    else
    {
        if(FAILED(D3DCompileFromFile(pixelShaderSource,
                                     NULL,
                                     NULL,
                                     pixelShaderEntryPoint,
                                     pixelShaderTarget,
                                     pixelShaderFlags1,
                                     pixelShaderFlags2,
                                     pixelShaderBuffer,
                                     &errorMessage)))
        {
            if(errorMessage)
            {
                OutputShaderErrorMessage(errorMessage);
            }
            else
            {
                System::GetInstance().ShowMessage(L"Shader::CreateShadersFromSource: Unable to locate pixel shader source",
                                                  L"Error");
            }

            success = false;
        }
    }

    if(success)
    {
        if(FAILED(device->CreateVertexShader((*vertexShaderBuffer)->GetBufferPointer(),
                                             (*vertexShaderBuffer)->GetBufferSize(),
                                             NULL,
                                             &m_vertexShader)))
        {
            System::GetInstance().ShowMessage(L"ColorShader::InitializeShader: Failed to create vertex shader",
                                              L"Error");
            success = false;
        }
        else
        {
            if(FAILED(device->CreatePixelShader((*pixelShaderBuffer)->GetBufferPointer(),
                                                (*pixelShaderBuffer)->GetBufferSize(),
                                                NULL,
                                                &m_pixelShader)))
            {
                System::GetInstance().ShowMessage(L"ColorShader::InitializeShader: Failed to create pixel shader",
                                                  L"Error");
                success = false;
            }
        }
    }

    return success;
}

void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage)
{
    char* errors = static_cast<char*>(errorMessage->GetBufferPointer());

    size_t bufferSize = errorMessage->GetBufferSize();

    std::ofstream fout;
    fout.open(SHADER_ERROR_LOG_FILE_NAME);

    for(unsigned int i = 0; i < bufferSize; ++i)
    {
        fout << errors[i];
    }

    fout.close();

    errorMessage->Release();
    errorMessage = NULL;

    System::GetInstance().ShowMessage(L"Shader::OutputShaderErrorMessage: Error compiling shader, see shader error log file",
                                      L"Error");
}