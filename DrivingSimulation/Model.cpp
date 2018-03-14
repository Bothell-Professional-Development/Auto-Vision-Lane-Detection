#include "stdafx.h"

#include <limits>

#include "Model.h"
#include "System.h"

Model::Model() :
    m_indexCount(0),
    m_vertexCount(0),
    m_position(NULL),
    m_texture(NULL),
    m_indexBuffer(NULL),
    m_vertexBuffer(NULL),
    m_color(1.0f, 1.0f, 1.0f, 1.0f),
    m_boundingBoxMinPoint(0.0f,
                          0.0f,
                          0.0f),
    m_boundingBoxMaxPoint(0.0f,
                          0.0f,
                          0.0f)
{
    m_position = new Position();
}
Model::Model(const Model& other){}
Model::~Model(){}

bool Model::Initialize(ID3D11Device* device,
                       ID3D11DeviceContext* context,
                       const char* modelFilename,
                       const char* textureFilename,
                       const Texture::TextureImageFormat imageFormat)
{
    bool success = false;

    if(LoadTexture(device, context, textureFilename, imageFormat))
    {
        if(LoadModel(modelFilename))
        {
            success = InitializeBuffers(device);
        }
    }

    return success;
}

bool Model::Initialize(ID3D11Device* device,
                       const char* modelFilename,
                       const DirectX::XMFLOAT4 color)
{
    bool success = false;

    if(LoadModel(modelFilename))
    {
        m_color = color;
        success = InitializeBuffers(device);
    }

    return success;
}

void Model::SetPosition(const float x,
                        const float y,
                        const float z)
{
    m_position->SetPosition(x, y, z);
}

void Model::SetRotation(const float x,
                        const float y,
                        const float z)
{
    m_position->SetRotation(x, y, z);
}

const DirectX::XMFLOAT3& Model::GetPosition() const
{
    return m_position->GetPosition();
}

const DirectX::XMFLOAT3& Model::GetRotation() const
{
    return m_position->GetRotation();
}

const DirectX::XMFLOAT3& Model::GetBoundingBoxMinPoint() const
{
    

    return m_boundingBoxMinPoint;
}

const DirectX::XMFLOAT3& Model::GetBoundingBoxMaxPoint() const
{
    return m_boundingBoxMaxPoint;
}

const DirectX::XMFLOAT3 Model::GetBoundingBoxDimensions() const
{
    return DirectX::XMFLOAT3(abs(m_boundingBoxMaxPoint.x - m_boundingBoxMinPoint.x),
                             abs(m_boundingBoxMaxPoint.y - m_boundingBoxMinPoint.y),
                             abs(m_boundingBoxMaxPoint.z - m_boundingBoxMinPoint.z));
}

const unsigned int& Model::GetIndexCount() const
{
    return m_indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture() const
{
    return m_texture == NULL ? NULL : m_texture->GetTexture();
}

const DirectX::XMFLOAT4& Model::GetColor() const
{
    return m_color;
}

void Model::Shutdown()
{
    if(m_position)
    {
        delete m_position;
        m_position = NULL;
    }

    ReleaseModel();
    ReleaseTexture();
    ShutdownBuffers();
}

void Model::ReverseFaces(ID3D11Device* device)
{
    for(unsigned int i = 0; i < m_vertexCount; i += 3)
    {
        if(m_texture)
        {
            TextureVertex temp = static_cast<TextureVertex*>(m_vertices)[i];
            static_cast<TextureVertex*>(m_vertices)[i] = static_cast<TextureVertex*>(m_vertices)[i + 2];
            static_cast<TextureVertex*>(m_vertices)[i + 2] = temp;
        }
        else
        {
            ColorVertex temp = static_cast<ColorVertex*>(m_vertices)[i];
            static_cast<ColorVertex*>(m_vertices)[i] = static_cast<ColorVertex*>(m_vertices)[i + 1];
            static_cast<ColorVertex*>(m_vertices)[i + 1] = temp;
        }
    }

    InitializeBuffers(device);
}

void Model::Render(ID3D11DeviceContext* context)
{
    RenderBuffers(context);
}

void Model::ShutdownBuffers()
{
    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = NULL;
    }

    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = NULL;
    }
}

void Model::RenderBuffers(ID3D11DeviceContext* context)
{
    if(m_vertexBuffer && m_indexBuffer)
    {
        unsigned int stride = m_texture ? sizeof(TextureVertex) : sizeof(ColorVertex);
        unsigned int offset = 0;

        context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }
}

bool Model::LoadTexture(ID3D11Device* device,
                        ID3D11DeviceContext* context,
                        const char* textureFilename,
                        const Texture::TextureImageFormat imageFormat)
{
    //just in case this model has been previously initialized
    ReleaseTexture();

    bool success = true;

    m_texture = new Texture();

    if(!m_texture->Initialize(device, context, textureFilename, imageFormat))
    {
        ReleaseTexture();
        success = false;
    }

    return success;
}

void Model::ReleaseTexture()
{
    if(m_texture)
    {
        m_texture->Shutdown();
        delete m_texture;
        m_texture = NULL;
    }
}

bool Model::LoadModel(const char* modelFilename)
{
    //just in case this model has already been initialized
    ReleaseModel();

    bool success = true;

    std::ifstream ifs;
    ifs.open(modelFilename);

    if(ifs.fail())
    {
        System::GetInstance().ShowMessage(L"Model::LoadModel: Failed to open model file",
                                          L"Error");
        success = false;
    }
    else
    {
        std::vector<DirectX::XMFLOAT3> vertices;
        std::vector<DirectX::XMFLOAT2> textureCoords;
        std::vector<DirectX::XMFLOAT3> normals;
        std::vector<ModelFace> faces;

        char data;
        ifs.get(data);

        while(!ifs.eof())
        {
            switch(data)
            {
                case 'v':
                ifs.get(data);

                switch(data)
                {
                    case ' ':
                    {
                        DirectX::XMFLOAT3 vertex;
                        ifs >> vertex.x >> vertex.y >> vertex.z;
                        
                        //assume right hand vertices, invert
                        vertex.z = -vertex.z;

                        vertices.push_back(vertex);
                    }
                    break;

                    case 't':
                    {
                        DirectX::XMFLOAT2 textureCoord;
                        ifs >> textureCoord.x >> textureCoord.y;

                        //assume right hand, invert
                        textureCoord.y = 1.0f - textureCoord.y;

                        textureCoords.push_back(textureCoord);
                    }
                    break;

                    case 'n':
                    {
                        DirectX::XMFLOAT3 normal;
                        ifs >> normal.x >> normal.y >> normal.z;

                        //assume right hand, invert
                        normal.z = -normal.z;

                        normals.push_back(normal);
                    }
                    break;

                    default:
                    break;
                }
                break;

                case 'f':
                ifs.get(data);

                if(data == ' ')
                {
                    ModelFace face;
                    char delimiter;

                    ifs >> face.vertex_2.x >> delimiter >> face.vertex_2.y >> delimiter >> face.vertex_2.z
                        >> face.vertex_1.x >> delimiter >> face.vertex_1.y >> delimiter >> face.vertex_1.z
                        >> face.vertex_0.x >> delimiter >> face.vertex_0.y >> delimiter >> face.vertex_0.z;

                    faces.push_back(face);
                }
                break;

                default:
                while(data != '\n')
                {
                    if(!ifs.eof())
                    {
                        ifs.get(data);
                    }
                    else
                    {
                        break;
                    }
                }
                break;
            }


            ifs.get(data);
        }

        ifs.close();

        if(faces.size() == 0)
        {
            System::GetInstance().ShowMessage(L"Model::LoadModel: Failed to read model or file contained no face data",
                                              L"Error");
            success = false;
        }

        if(m_texture)
        {
            m_vertices = new TextureVertex[faces.size() * 3];
        }
        else
        {
            m_vertices = new ColorVertex[faces.size() * 3];
        }

        for(unsigned int i = 0; i < faces.size(); ++i)
        {
            if(!AddVertex(faces[i].vertex_0.x - 1,
                          faces[i].vertex_0.y - 1,
                          faces[i].vertex_0.z - 1,
                          vertices,
                          textureCoords,
                          normals))
            {
                success = false;
                break;
            }

            if(!AddVertex(faces[i].vertex_1.x - 1,
                          faces[i].vertex_1.y - 1,
                          faces[i].vertex_1.z - 1,
                          vertices,
                          textureCoords,
                          normals))
            {
                success = false;
                break;
            }

            if(!AddVertex(faces[i].vertex_2.x - 1,
                          faces[i].vertex_2.y - 1,
                          faces[i].vertex_2.z - 1,
                          vertices,
                          textureCoords,
                          normals))
            {
                success = false;
                break;
            }
        }
    }

    return success;
}

bool Model::AddVertex(const unsigned int vIndex,
                      const unsigned int tIndex,
                      const unsigned int nIndex,
                      const std::vector<DirectX::XMFLOAT3>& vertices,
                      const std::vector<DirectX::XMFLOAT2>& textureCoords,
                      const std::vector<DirectX::XMFLOAT3>& normals)
{
    bool success = true;

    if(vIndex < vertices.size() &&
       tIndex < textureCoords.size() &&
       nIndex < normals.size())
    {
        if(m_texture)
        {
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].position.x = vertices[vIndex].x;
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].position.y = vertices[vIndex].y;
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].position.z = vertices[vIndex].z;
            
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].texture.x = textureCoords[tIndex].x;
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].texture.y = textureCoords[tIndex].y;
            
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].normal.x = normals[nIndex].x;
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].normal.y = normals[nIndex].y;
            static_cast<TextureVertex*>(m_vertices)[m_vertexCount].normal.z = normals[nIndex].z;
        }
        else
        {
            static_cast<ColorVertex*>(m_vertices)[m_vertexCount].position.x = vertices[vIndex].x;
            static_cast<ColorVertex*>(m_vertices)[m_vertexCount].position.y = vertices[vIndex].y;
            static_cast<ColorVertex*>(m_vertices)[m_vertexCount].position.z = vertices[vIndex].z;

            static_cast<ColorVertex*>(m_vertices)[m_vertexCount].color.w = m_color.w;
            static_cast<ColorVertex*>(m_vertices)[m_vertexCount].color.x = m_color.x;
            static_cast<ColorVertex*>(m_vertices)[m_vertexCount].color.y = m_color.y;
            static_cast<ColorVertex*>(m_vertices)[m_vertexCount].color.z = m_color.z;
        }

        ++m_vertexCount;
        ++m_indexCount;
    }
    else
    {
        System::GetInstance().ShowMessage(L"Model::AddVertex: Face references non-existent data",
                                          L"Error");
        success = false;
    }
    
    return success;
}

void Model::ReleaseModel()
{
    if(m_vertices)
    {
        delete[] m_vertices;
        m_vertices = NULL;

        m_vertexCount = 0;
    }

    if(m_indices)
    {
        delete[] m_indices;
        m_indices = NULL;

        m_indexCount = 0;
    }
}

void Model::UpdateBoundingBox()
{
    m_boundingBoxMinPoint.x = (std::numeric_limits<float>::max)();
    m_boundingBoxMinPoint.y = (std::numeric_limits<float>::max)();
    m_boundingBoxMinPoint.z = (std::numeric_limits<float>::max)();
    m_boundingBoxMaxPoint.x = std::numeric_limits<float>::lowest();
    m_boundingBoxMaxPoint.y = std::numeric_limits<float>::lowest();
    m_boundingBoxMaxPoint.z = std::numeric_limits<float>::lowest();

    for(unsigned int i = 0; i < m_vertexCount; ++i)
    {
        float x;
        float y;
        float z;

        if(m_texture)
        {
            TextureVertex vertex = static_cast<TextureVertex*>(m_vertices)[i];

            x = vertex.position.x;
            y = vertex.position.y;
            z = vertex.position.z;
        }
        else
        {
            ColorVertex vertex = static_cast<ColorVertex*>(m_vertices)[i];

            x = vertex.position.x;
            y = vertex.position.y;
            z = vertex.position.z;
        }

        m_boundingBoxMinPoint.x = x < m_boundingBoxMinPoint.x ? x : m_boundingBoxMinPoint.x;
        m_boundingBoxMinPoint.y = y < m_boundingBoxMinPoint.y ? y : m_boundingBoxMinPoint.y;
        m_boundingBoxMinPoint.z = z < m_boundingBoxMinPoint.z ? z : m_boundingBoxMinPoint.z;
        m_boundingBoxMaxPoint.x = x > m_boundingBoxMaxPoint.x ? x : m_boundingBoxMaxPoint.x;
        m_boundingBoxMaxPoint.y = y > m_boundingBoxMaxPoint.y ? y : m_boundingBoxMaxPoint.y;
        m_boundingBoxMaxPoint.z = z > m_boundingBoxMaxPoint.z ? z : m_boundingBoxMaxPoint.z;
    }
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
    //just in case this model has already been initialized
    ShutdownBuffers();

    bool success = true;

    void* vertices = NULL;
    m_indices = new unsigned int[m_indexCount];

    if(m_texture)
    {
        vertices = new TextureVertex[m_vertexCount];

        for(unsigned int i = 0; i < m_vertexCount; ++i)
        {
            static_cast<TextureVertex*>(vertices)[i].position = static_cast<TextureVertex*>(m_vertices)[i].position;
            static_cast<TextureVertex*>(vertices)[i].texture = static_cast<TextureVertex*>(m_vertices)[i].texture;
            static_cast<TextureVertex*>(vertices)[i].normal = static_cast<TextureVertex*>(m_vertices)[i].normal;

            m_indices[i] = i;
        }
    }
    else
    {
        vertices = new ColorVertex[m_vertexCount];

        for(unsigned int i = 0; i < m_vertexCount; ++i)
        {
            static_cast<ColorVertex*>(vertices)[i].position = static_cast<ColorVertex*>(m_vertices)[i].position;
            static_cast<ColorVertex*>(vertices)[i].color = static_cast<ColorVertex*>(m_vertices)[i].color;

            m_indices[i] = i;
        }
    }

    D3D11_BUFFER_DESC vertexBufferDesc;

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = m_vertexCount * (m_texture ? sizeof(TextureVertex) : sizeof(ColorVertex));
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexData;

    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    if(FAILED(device->CreateBuffer(&vertexBufferDesc,
                                   &vertexData,
                                   &m_vertexBuffer)))
    {
        System::GetInstance().ShowMessage(L"Model::InitializeBuffers: Failed to create vertex buffer",
                                          L"Error");
        success = false;
    }

    if(vertices)
    {
        delete[] vertices;
        vertices = NULL;
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
            System::GetInstance().ShowMessage(L"Model::InitializeBuffers: Failed to create index buffer",
                                              L"Error");
            success = false;
        }
    }

    if(!success)
    {
        ShutdownBuffers();
    }
    else
    {
        UpdateBoundingBox();
    }

    return success;
}