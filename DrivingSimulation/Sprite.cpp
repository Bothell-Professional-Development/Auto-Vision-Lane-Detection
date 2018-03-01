#include "stdafx.h"

#include "Sprite.h"
#include "System.h"

Sprite::Sprite() :
    m_width(0),
    m_height(0){}
Sprite::Sprite(const Sprite& other){}
Sprite::~Sprite(){}

bool Sprite::Initialize(ID3D11Device* device,
                        ID3D11DeviceContext* context,
                        const float height,
                        const float width,
                        const char* textureFilename,
                        const Texture::TextureImageFormat imageFormat)
{
    bool success = false;

    m_width = width;
    m_height = height;
    m_vertexCount = 6;
    m_indexCount = m_vertexCount;

    if(LoadTexture(device, context, textureFilename, imageFormat))
    {
        success = InitializeBuffers(device);
    }

    return success;
}

void Sprite::Shutdown()
{
    Model::Shutdown();
}

void Sprite::Render(ID3D11DeviceContext* context)
{
    if(m_vertexBuffer && m_indexBuffer)
    {
        unsigned int stride = sizeof(TextureVertex);
        unsigned int offset = 0;

        context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
}

bool Sprite::InitializeBuffers(ID3D11Device* device)
{
    bool success = true;

    //just in case this model has already been initialized
    ShutdownBuffers();

    m_vertices = new TextureVertex[m_vertexCount];
    m_indices = new unsigned int[m_indexCount];

    //first poly
    static_cast<TextureVertex*>(m_vertices)[0].position = DirectX::XMFLOAT3(-m_width / 2,
                                                                            m_height / 2,
                                                                            0.0f);
    static_cast<TextureVertex*>(m_vertices)[0].texture = DirectX::XMFLOAT2(0.0f,
                                                                           0.0f);
    static_cast<TextureVertex*>(m_vertices)[0].normal = DirectX::XMFLOAT3(0.0f,
                                                                          0.0f,
                                                                          -1.0f);

    static_cast<TextureVertex*>(m_vertices)[1].position = DirectX::XMFLOAT3(m_width / 2,
                                                                            -m_height / 2,
                                                                            0.0f);
    static_cast<TextureVertex*>(m_vertices)[1].texture = DirectX::XMFLOAT2(1.0f,
                                                                           1.0f);
    static_cast<TextureVertex*>(m_vertices)[1].normal = DirectX::XMFLOAT3(0.0f,
                                                                          0.0f,
                                                                          -1.0f);

    static_cast<TextureVertex*>(m_vertices)[2].position = DirectX::XMFLOAT3(-m_width / 2,
                                                                            -m_height / 2,
                                                                            0.0f);
    static_cast<TextureVertex*>(m_vertices)[2].texture = DirectX::XMFLOAT2(0.0f,
                                                                           1.0f);
    static_cast<TextureVertex*>(m_vertices)[2].normal = DirectX::XMFLOAT3(0.0f,
                                                                          0.0f,
                                                                          -1.0f);

    //second poly
    static_cast<TextureVertex*>(m_vertices)[3].position = DirectX::XMFLOAT3(-m_width / 2,
                                                                            m_height / 2,
                                                                            0.0f);
    static_cast<TextureVertex*>(m_vertices)[3].texture = DirectX::XMFLOAT2(0.0f,
                                                                           0.0f);
    static_cast<TextureVertex*>(m_vertices)[3].normal = DirectX::XMFLOAT3(0.0f,
                                                                          0.0f,
                                                                          -1.0f);

    static_cast<TextureVertex*>(m_vertices)[4].position = DirectX::XMFLOAT3(m_width / 2,
                                                                            m_height / 2,
                                                                            0.0f);
    static_cast<TextureVertex*>(m_vertices)[4].texture = DirectX::XMFLOAT2(1.0f,
                                                                           0.0f);
    static_cast<TextureVertex*>(m_vertices)[4].normal = DirectX::XMFLOAT3(0.0f,
                                                                          0.0f,
                                                                          -1.0f);

    static_cast<TextureVertex*>(m_vertices)[5].position = DirectX::XMFLOAT3(m_width / 2,
                                                                            -m_height / 2,
                                                                            0.0f);
    static_cast<TextureVertex*>(m_vertices)[5].texture = DirectX::XMFLOAT2(1.0f,
                                                                           1.0f);
    static_cast<TextureVertex*>(m_vertices)[5].normal = DirectX::XMFLOAT3(0.0f,
                                                                          0.0f,
                                                                          -1.0f);

    return Model::InitializeBuffers(device);
}