#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Model.h"

class Terrain : public Model
{
public:
    Terrain();
    Terrain(const Terrain& other);
    ~Terrain();

    bool Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context,
                    const unsigned int width,
                    const unsigned int depth,
                    const char* textureFilename,
                    const Texture::TextureImageFormat imageFormat);

    bool Initialize(ID3D11Device* device,
                    const unsigned int width,
                    const unsigned int depth,
                    const DirectX::XMFLOAT4 color);

    bool Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context,
                    const char* textureFilename,
                    const Texture::TextureImageFormat imageFormat,
                    const char* heightMapFilename,
                    const Texture::TextureImageFormat heightMapFormat);

    bool Initialize(ID3D11Device* device,
                    const DirectX::XMFLOAT4 color,
                    const char* heightMapFilename,
                    const Texture::TextureImageFormat heightMapFormat);

    virtual void Shutdown();

private:
    bool LoadHeightMap(const char* heightMapFilename,
                       const Texture::TextureImageFormat heightMapFormat);

    bool LoadTargaHeightMap(const char* heightMapFilename);

    //void CalculateNormals();

    virtual bool InitializeBuffers(ID3D11Device* device);

    unsigned int m_width;
    unsigned int m_depth;

    float* m_heightMap;
};
#pragma once