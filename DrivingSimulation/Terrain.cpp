#include "stdafx.h"

#include "Terrain.h"
#include "System.h"

Terrain::Terrain() :
    m_width(0),
    m_depth(0){}
Terrain::Terrain(const Terrain& other){}
Terrain::~Terrain(){}

bool Terrain::Initialize(ID3D11Device* device,
                         ID3D11DeviceContext* context,
                         const unsigned int width,
                         const unsigned int depth,
                         const char* textureFilename,
                         const Texture::TextureImageFormat imageFormat)
{
    m_width = width;
    m_depth = depth;
    m_vertexCount = m_width * m_depth * 6;
    m_indexCount = m_vertexCount;

    bool success = false;

    if(LoadTexture(device, context, textureFilename, imageFormat))
    {
        success = InitializeBuffers(device);
    }

    return success;
}

bool Terrain::Initialize(ID3D11Device* device,
                         const unsigned int width,
                         const unsigned int depth,
                         const DirectX::XMFLOAT4 color)
{
    m_width = width;
    m_depth = depth;
    m_color = color;
    m_vertexCount = m_width * m_depth * 6;
    m_indexCount = m_vertexCount;

    return InitializeBuffers(device);
}

void Terrain::Shutdown()
{
    m_width = 0;
    m_depth = 0;

    Model::Shutdown();
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
    bool success = true;

    if(m_texture)
    {
        m_vertices = new TextureVertex[m_vertexCount];
    }
    else
    {
        m_vertices = new ColorVertex[m_vertexCount];
    }

    m_indices = new unsigned int[m_indexCount];

    int outIndex = 0;
    int depthStartIndex = -static_cast<int>(m_depth / 2);
    int widthStartIndex = -static_cast<int>(m_width / 2);

    for(int i = depthStartIndex; i < depthStartIndex + m_depth; ++i)
    {
        for(int j = widthStartIndex; j < widthStartIndex + m_width; ++j)
        {
            if(m_texture)
            {
                //Triangle 1 - Rear left
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Rear right
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Front left
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Front left
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Rear right
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Front right
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i- 0.5f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
                m_indices[outIndex] = outIndex;

                ++outIndex;
            }
            else
            {
                //Triangle 1 - Rear left
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Rear right
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Front left
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Front left
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Rear right
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Front right
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;
            }
        }
    }

    return Model::InitializeBuffers(device);
}