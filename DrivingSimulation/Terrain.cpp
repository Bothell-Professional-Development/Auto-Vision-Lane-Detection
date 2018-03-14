#include "stdafx.h"

#include "Geometry.h"
#include "Terrain.h"
#include "System.h"

Terrain::Terrain() :
    m_width(0),
    m_depth(0),
    m_heightMap(NULL){}
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

bool Terrain::Initialize(ID3D11Device* device,
                         ID3D11DeviceContext* context,
                         const char* textureFilename,
                         const Texture::TextureImageFormat imageFormat,
                         const char* heightMapFilename,
                         const Texture::TextureImageFormat heightMapFormat)
{
    bool success = true;

    if(LoadTexture(device, context, textureFilename, imageFormat))
    {
        success &= LoadHeightMap(heightMapFilename, heightMapFormat);

        if(success)
        {
            m_vertexCount = m_width * m_depth * 6;
            m_indexCount = m_vertexCount;
        }

        success &= InitializeBuffers(device);
    }
    else
    {
        success = false;
    }

    return success;
}

bool Terrain::Initialize(ID3D11Device* device,
                         const DirectX::XMFLOAT4 color,
                         const char* heightMapFilename,
                         const Texture::TextureImageFormat heightMapFormat)
{
    bool success = true;

    if(LoadHeightMap(heightMapFilename, heightMapFormat))
    {
        m_vertexCount = m_width * m_depth * 6;
        m_indexCount = m_vertexCount;

        success &= InitializeBuffers(device);
    }
    else
    {
        success = false;
    }

    return success;
}

void Terrain::Shutdown()
{
    m_width = 0;
    m_depth = 0;

    if(m_heightMap)
    {
        delete[] m_heightMap;
        m_heightMap = NULL;
    }

    Model::Shutdown();
}

bool Terrain::LoadHeightMap(const char* heightMapFilename,
                            const Texture::TextureImageFormat heightMapFormat)
{
    bool success = true;

    switch(heightMapFormat)
    {
        case Texture::TextureImageFormat::TARGA:
            success = LoadTargaHeightMap(heightMapFilename);
            break;
        default:
            success = false;
            break;
    }
    
    return success;
}

bool Terrain::LoadTargaHeightMap(const char* heightMapFilename)
{
    if(m_heightMap)
    {
        delete[] m_heightMap;
        m_heightMap = NULL;
    }

    bool success = true;

    FILE* file = NULL;
    size_t readSize;

    if(fopen_s(&file, heightMapFilename, "rb") != 0)
    {
        System::GetInstance().ShowMessage(L"Terrain::LoadTargaHeightMap: Failed to load TGA height map",
                                          L"Error");
        success = false;
    }
    else
    {
        Texture::TargaHeader targaHeader;
        readSize = fread(&targaHeader, sizeof(Texture::TargaHeader), 1, file);

        if(readSize != 1)
        {
            System::GetInstance().ShowMessage(L"Terrain::LoadTargaHeightMap: Failed to read TGA header",
                                              L"Error");
            success = false;
        }
        else
        {
            m_width = targaHeader.width - 1;
            m_depth = targaHeader.height - 1;
            
            if(targaHeader.bpp != 8)
            {
                System::GetInstance().ShowMessage(L"Terrain::LoadTargaHeightMap: TGA color depth unsupported",
                                                  L"Error");
                success = false;
            }
        }
    }

    if(success)
    {
        unsigned int imageWidth = m_width + 1;
        unsigned int imageHeight = m_depth + 1;

        unsigned int imageSize = imageWidth * imageHeight;
        unsigned char* data = new unsigned char[imageSize];

        m_heightMap = new float[imageSize];
        ZeroMemory(m_heightMap, sizeof(float) * imageSize);

        readSize = fread(data, 1, imageSize, file);

        if(readSize != imageSize)
        {
            System::GetInstance().ShowMessage(L"Terrain::LoadTargaHeightMap: Error occurred while reading image data",
                                              L"Error");
            success = false;
        }
        else
        {
            unsigned int writeIndex = 0;
            unsigned int readIndex = (imageWidth * imageHeight) - imageWidth;

            int black = 0;

            for(unsigned int i = 0; i < m_depth; ++i)
            {
                for(unsigned int j = 0; j < m_width; ++j)
                {
                    m_heightMap[writeIndex] = data[readIndex] / 15.0f;

                    if(data[readIndex] = 0.0f)
                    {
                        ++black;
                    }

                    ++writeIndex;
                    ++readIndex;
                }

                readIndex -= imageWidth * 2 - 1;
            }
            ++black;
        }

        delete[] data;
        data = NULL;
    }

    return success;
}

//void Terrain::CalculateNormals()
//{
//    for(unsigned int i = 0; i < m_depth - 1; ++i)
//    {
//        for(unsigned int j = 0; j < m_width - 1; ++j)
//        {
//            unsigned int index1 = (m_depth * i) + j;
//            unsigned int index2 = (m_depth * i) + j + 1;
//            unsigned int index3 = (m_depth * (i + 1)) + j;
//
//            float h1 = m_heightMap[index1].height;
//            float h2 = m_heightMap[index2].height;
//            float h3 = m_heightMap[index3].height;
//
//            DirectX::XMFLOAT3 normal = Geometry::CalculateNormal(DirectX::XMFLOAT3(0.0f,
//                                                                                   m_heightMap[index1].height,
//                                                                                   0.0f),
//                                                                 DirectX::XMFLOAT3(1.0f,
//                                                                                   m_heightMap[index2].height,
//                                                                                   0.0f),
//                                                                 DirectX::XMFLOAT3(0.0f,
//                                                                                   m_heightMap[index3].height,
//                                                                                   1.0f));
//            
//            float h1 = m_heightMap[index1].height;
//            float h2 = m_heightMap[index2].height;
//            float h3 = m_heightMap[index3].height;
//        }
//    }
//}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
    bool success = true;

    if(!m_heightMap)
    {
        m_heightMap = new float[(m_width + 1) * (m_depth + 1)];
        ZeroMemory(m_heightMap, sizeof(float) * (m_width + 1) * (m_depth + 1));
    }

    if(m_texture)
    {
        m_vertices = new TextureVertex[m_vertexCount];
    }
    else
    {
        m_vertices = new ColorVertex[m_vertexCount];
    }

    m_indices = new unsigned int[m_indexCount];

    unsigned int outIndex = 0;
    int depthStartIndex = static_cast<int>(m_depth / 2);
    int widthStartIndex = -static_cast<int>(m_width / 2);

    for(int i = 0; i < m_depth; ++i)
    {
        for(int j = 0; j < m_width; ++j)
        {
            unsigned int rearLeftIndex = (m_depth * i) + j;
            unsigned int rearRightIndex = (m_depth * i) + j + 1;
            unsigned int frontLeftIndex = (m_depth * (i + 1)) + j;
            unsigned int frontRightIndex = (m_depth * (i + 1)) + j + 1;

            if(m_texture)
            {
                DirectX::XMFLOAT3 normal = Geometry::CalculateNormal(DirectX::XMFLOAT3(0.0f,
                                                                                       m_heightMap[rearLeftIndex],
                                                                                       0.0f),
                                                                     DirectX::XMFLOAT3(1.0f,
                                                                                       m_heightMap[rearRightIndex],
                                                                                       0.0f),
                                                                     DirectX::XMFLOAT3(0.0f,
                                                                                       m_heightMap[frontLeftIndex],
                                                                                       1.0f));

                //Triangle 1 - Rear left
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j);
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = m_heightMap[rearLeftIndex];
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i);
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal = normal;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Rear right
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j + 1);
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = m_heightMap[rearRightIndex];
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i);
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal = normal;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Front left
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j);
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = m_heightMap[frontLeftIndex];
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i - 1);
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal = normal;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                normal = Geometry::CalculateNormal(DirectX::XMFLOAT3(0.0f,
                                                                     m_heightMap[frontLeftIndex],
                                                                     1.0f),
                                                   DirectX::XMFLOAT3(1.0f,
                                                                     m_heightMap[rearRightIndex],
                                                                     0.0f),
                                                   DirectX::XMFLOAT3(1.0f,
                                                                     m_heightMap[frontRightIndex],
                                                                     1.0f));

                //Triangle 2 - Front left
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j);
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = m_heightMap[frontLeftIndex];
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i - 1);
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal = normal;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Rear right
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j + 1);
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = m_heightMap[rearRightIndex];
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i);
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal = normal;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Front right
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j + 1);
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = m_heightMap[frontRightIndex];
                static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i - 1);
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
                static_cast<TextureVertex*>(m_vertices)[outIndex].normal = normal;
                m_indices[outIndex] = outIndex;

                ++outIndex;
            }
            else
            {
                //Triangle 1 - Rear left
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j);
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = m_heightMap[rearLeftIndex];
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i);
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Rear right
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j + 1);
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = m_heightMap[rearRightIndex];
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i);
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 1 - Front left
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j);
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = m_heightMap[frontRightIndex];
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i - 1);
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Front left
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j);
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = m_heightMap[frontLeftIndex];
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i - 1);
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Rear right
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j + 1);
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = m_heightMap[rearRightIndex];
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i);
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;

                //Triangle 2 - Front right
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = static_cast<float>(widthStartIndex + j + 1);
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = m_heightMap[frontLeftIndex];
                static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = static_cast<float>(depthStartIndex - i - 1);
                static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
                m_indices[outIndex] = outIndex;

                ++outIndex;
            }
        }
    }

    //int depthStartIndex = -static_cast<int>(m_depth / 2);
    //int widthStartIndex = -static_cast<int>(m_width / 2);

    //for(int i = depthStartIndex; i < static_cast<int>(depthStartIndex + m_depth); ++i)
    //{
    //    for(int j = widthStartIndex; j < static_cast<int>(widthStartIndex + m_width); ++j)
    //    {
    //        if(m_texture)
    //        {
    //            //Triangle 1 - Rear left
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 1 - Rear right
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 1 - Front left
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 2 - Front left
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 2 - Rear right
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 2 - Front right
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].position.z = i- 0.5f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.x = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].texture.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.x = 0.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.y = 1.0f;
    //            static_cast<TextureVertex*>(m_vertices)[outIndex].normal.z = 0.0f;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;
    //        }
    //        else
    //        {
    //            //Triangle 1 - Rear left
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 1 - Rear right
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 1 - Front left
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 2 - Front left
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j - 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 2 - Rear right
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i + 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;

    //            //Triangle 2 - Front right
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.x = j + 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.y = 0.0f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].position.z = i - 0.5f;
    //            static_cast<ColorVertex*>(m_vertices)[outIndex].color = m_color;
    //            m_indices[outIndex] = outIndex;

    //            ++outIndex;
    //        }
    //    }
    //}

    return Model::InitializeBuffers(device);
}