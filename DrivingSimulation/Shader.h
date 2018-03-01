#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

class Shader
{
public:
    Shader();
    Shader(const Shader& other);
    ~Shader();

    virtual bool Initialize(ID3D11Device* device) = 0;
    virtual void Shutdown();

protected:
    struct MatrixBuffer
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    bool CreateShadersFromSource(ID3D11Device* device,
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
                                 ID3D10Blob** pixelShaderBuffer);

    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_layout;
    ID3D11Buffer* m_matrixBuffer;

private:
    virtual bool InitializeShader(ID3D11Device* device,
                                  const WCHAR* vertexShaderSource,
                                  const WCHAR* pixelShaderSource) = 0;

    virtual void RenderShader(ID3D11DeviceContext* context,
                              const unsigned int indexCount) = 0;
    
    void OutputShaderErrorMessage(ID3D10Blob* errorMessage);
    
    static const std::string SHADER_ERROR_LOG_FILE_NAME;
};