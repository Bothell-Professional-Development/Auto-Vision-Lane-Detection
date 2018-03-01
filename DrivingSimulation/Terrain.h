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

    virtual void Shutdown();

private:
    virtual bool InitializeBuffers(ID3D11Device* device);

    int m_width;
    int m_depth;
};
#pragma once