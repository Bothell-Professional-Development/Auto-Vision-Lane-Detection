#pragma once

#include "Model.h"

class Sprite : public Model
{
public:
    Sprite();
    Sprite(const Sprite& other);
    ~Sprite();

    bool Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context,
                    const float width,
                    const float height,
                    const char* textureFilename,
                    const Texture::TextureImageFormat imageFormat);

    virtual void Shutdown();
    virtual void Render(ID3D11DeviceContext* context);

private:
    virtual bool InitializeBuffers(ID3D11Device* device);

    float m_width;
    float m_height;
};