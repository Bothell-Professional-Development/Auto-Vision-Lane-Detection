#pragma once

#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Position.h"
#include "Texture.h"
#include "Renderable.h"

class Model : public Renderable
{
public:
    Model();
    Model(const Model& other);
    ~Model();

    bool Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context,
                    const char* modelFilename,
                    const char* textureFilename,
                    const Texture::TextureImageFormat imageFormat);

    bool Initialize(ID3D11Device* device,
                    const char* modelFilename,
                    const DirectX::XMFLOAT4 color);

    void SetPosition(const float x,
                     const float y,
                     const float z);

    void SetRotation(const float x,
                     const float y,
                     const float z);

    const DirectX::XMFLOAT3& GetPosition() const;
    const DirectX::XMFLOAT3& GetRotation() const;

    const unsigned int& GetIndexCount() const ;
    ID3D11ShaderResourceView* GetTexture() const;
    const DirectX::XMFLOAT4& GetColor() const;

    virtual void Shutdown();
    virtual void Render(ID3D11DeviceContext* context);

protected:
    struct ColorVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

    struct TextureVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };

    struct ModelFace
    {
        DirectX::XMUINT3 vertex_0;
        DirectX::XMUINT3 vertex_1;
        DirectX::XMUINT3 vertex_2;
    };

    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext* context);

    bool LoadTexture(ID3D11Device* device,
                     ID3D11DeviceContext* context,
                     const char* textureFilename,
                     const Texture::TextureImageFormat imageFormat);

    void ReleaseTexture();
    bool LoadModel(const char* modelFilename);

    bool AddVertex(const unsigned int vIndex,
                           const unsigned int tIndex,
                           const unsigned int nIndex,
                           const std::vector<DirectX::XMFLOAT3>& vertices,
                           const std::vector<DirectX::XMFLOAT2>& textureCoords,
                           const std::vector<DirectX::XMFLOAT3>& normals);

    void ReleaseModel();

    virtual bool InitializeBuffers(ID3D11Device* device);

    unsigned int m_indexCount;
    unsigned int m_vertexCount;

    Position* m_position;
    Texture* m_texture;

    ID3D11Buffer* m_indexBuffer;
    ID3D11Buffer* m_vertexBuffer;
    DirectX::XMFLOAT4 m_color;
    void* m_vertices;
    unsigned int* m_indices;
};