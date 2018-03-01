#include "stdafx.h"

#include "Text.h"
#include "System.h"

Text::Text() :
    m_screenWidth(0),
    m_screenHeight(0),
    m_maxLength(0),
    m_font(NULL){}
Text::Text(const Text& other){}
Text::~Text(){}

bool Text::Initialize(ID3D11Device* device,
                      ID3D11DeviceContext* context,
                      const unsigned int screenWidth,
                      const unsigned int screenHeight,
                      const char* textureFilename,
                      const Texture::TextureImageFormat imageFormat)
{
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    m_font = new Font();

    m_font->Initialize(device, context);

    return LoadTexture(device,
                       context,
                       textureFilename,
                       imageFormat);
}

void Text::Shutdown()
{
    if(m_font)
    {
        m_font->Shutdown();
        delete m_font;
        m_font = NULL;
    }

    Model::Shutdown();
}

void Text::Render(ID3D11DeviceContext* context)
{
    if(m_vertexBuffer && m_indexBuffer)
    {
        unsigned int stride = sizeof(GlyphVertex);
        unsigned int offset = 0;

        context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
}

bool Text::InitializeBuffers(ID3D11Device* device)
{
    bool success = true;

    //just in case this model has already been initialized
    ShutdownBuffers();
    
    m_vertices = new GlyphVertex[m_vertexCount];

    m_indices = new unsigned int[m_indexCount];

    for(unsigned int i = 0; i < m_indexCount; ++i)
    {
        m_indices[i] = i;
    }

    D3D11_BUFFER_DESC vertexBufferDesc;

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(GlyphVertex) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;

    vertexData.pSysMem = m_vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    if(FAILED(device->CreateBuffer(&vertexBufferDesc,
                                   &vertexData,
                                   &m_vertexBuffer)))
    {
        System::GetInstance().ShowMessage(L"Text::InitializeBuffers: Failed to create vertex buffer",
                                          L"Error");
        success = false;
    }

    if(success)
    {
        D3D11_BUFFER_DESC indexBufferDesc;

        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA indexData;

        indexData.pSysMem = m_indices;
        indexData.SysMemPitch = 0;
        indexData.SysMemSlicePitch = 0;

        if(FAILED(device->CreateBuffer(&indexBufferDesc,
                                       &indexData,
                                       &m_indexBuffer)))
        {
            System::GetInstance().ShowMessage(L"Text::InitializeBuffers: Failed to create index buffer",
                                              L"Error");
            success = false;
        }
    }

    return success;
}

bool Text::InitializeText(ID3D11Device* device,
                          const unsigned int maxLength)
{
    m_maxLength = maxLength;
    m_vertexCount = m_maxLength * 6;
    m_indexCount = m_vertexCount;

    return InitializeBuffers(device);
}

bool Text::UpdateText(ID3D11DeviceContext* context,
                      const char* text,
                      const unsigned int screenX,
                      const unsigned int screenY,
                      const DirectX::XMFLOAT4 color)
{
    bool success = true;

    m_color = color;

    size_t size = strlen(text);

    if(size > m_maxLength)
    {
        System::GetInstance().ShowMessage(L"Text::UpdateText: Vertex buffer too small to render desired string",
                                          L"Error");
        success = false;
    }

    if(success)
    {
        memset(m_vertices, 0, sizeof(GlyphVertex) * m_vertexCount);

        //convert screen coords to coords in 3D coordinate system
        float renderX = (-1.0f * (m_screenWidth / 2)) + screenX;
        float renderY = static_cast<float>(m_screenHeight / 2) - screenY;

        success = UpdateVertexArray(text,
                                    renderX,
                                    renderY);
    }

    if(success)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        if(FAILED(context->Map(m_vertexBuffer,
                               0,
                               D3D11_MAP_WRITE_DISCARD,
                               0,
                               &mappedResource)))
        {
            System::GetInstance().ShowMessage(L"Text::UpdateText: Failed to map vertex buffer",
                                              L"Error");
            success = false;
        }
        else
        {
            memcpy(static_cast<GlyphVertex*>(mappedResource.pData),
                   m_vertices,
                   sizeof(GlyphVertex) * m_vertexCount);

            context->Unmap(m_vertexBuffer, 0);
        }
    }

    return success;
}

bool Text::UpdateVertexArray(const char* sentence,
                             const float renderX,
                             const float renderY)
{
    bool success = true;

    size_t size = strlen(sentence);
    unsigned int outIndex = 0;

    float x = renderX;
    float y = renderY;

    Font::Glyph glyph;    

    for(unsigned int i = 0; i < size; ++i)
    {
        success = m_font->GetGlyph(sentence[i], glyph);
           
        if(success)
        {
            if(glyph.right - glyph.left > 0.0f)
            {
                //first poly
                static_cast<GlyphVertex*>(m_vertices)[outIndex].position = DirectX::XMFLOAT3(x,
                                                                                             y,
                                                                                             0.0f);
                static_cast<GlyphVertex*>(m_vertices)[outIndex].texture = DirectX::XMFLOAT2(glyph.left,
                                                                                            0.0f);
                ++outIndex;

                static_cast<GlyphVertex*>(m_vertices)[outIndex].position = DirectX::XMFLOAT3(x + glyph.size,
                                                                                             y - 16.0f,
                                                                                             0.0f);
                static_cast<GlyphVertex*>(m_vertices)[outIndex].texture = DirectX::XMFLOAT2(glyph.right,
                                                                                            1.0f);
                ++outIndex;

                static_cast<GlyphVertex*>(m_vertices)[outIndex].position = DirectX::XMFLOAT3(x,
                                                                                             y - 16.0f,
                                                                                             0.0f);
                static_cast<GlyphVertex*>(m_vertices)[outIndex].texture = DirectX::XMFLOAT2(glyph.left,
                                                                                            1.0f);

                ++outIndex;

                //second poly
                static_cast<GlyphVertex*>(m_vertices)[outIndex].position = DirectX::XMFLOAT3(x,
                                                                                             y,
                                                                                             0.0f);
                static_cast<GlyphVertex*>(m_vertices)[outIndex].texture = DirectX::XMFLOAT2(glyph.left,
                                                                                            0.0f);
                ++outIndex;

                static_cast<GlyphVertex*>(m_vertices)[outIndex].position = DirectX::XMFLOAT3(x + glyph.size,
                                                                                             y,
                                                                                             0.0f);
                static_cast<GlyphVertex*>(m_vertices)[outIndex].texture = DirectX::XMFLOAT2(glyph.right,
                                                                                            0.0f);
                ++outIndex;

                static_cast<GlyphVertex*>(m_vertices)[outIndex].position = DirectX::XMFLOAT3(x + glyph.size,
                                                                                             y - 16.0f,
                                                                                             0.0f);
                static_cast<GlyphVertex*>(m_vertices)[outIndex].texture = DirectX::XMFLOAT2(glyph.right,
                                                                                            1.0f);

                ++outIndex;
            }

            x += glyph.size + 1.0f;
        }
        else
        {
            break;
        }
    }

    return success;
}