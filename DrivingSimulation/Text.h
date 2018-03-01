#pragma once

#include "Font.h"
#include "Model.h"

class Text : public Model
{
public:

    Text();
    Text(const Text& other);
    ~Text();

    bool Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context,
                    const unsigned int screenWidth,
                    const unsigned int screenHeight,
                    const char* textureFilename,
                    const Texture::TextureImageFormat imageFormat);

    bool InitializeText(ID3D11Device* device,
                        const unsigned int maxLength);

    bool UpdateText(ID3D11DeviceContext* context,
                    const char* text,
                    const unsigned int screenX,
                    const unsigned int screenY,
                    const DirectX::XMFLOAT4 color);

    virtual void Shutdown();
    virtual void Render(ID3D11DeviceContext* context);

private:
    struct GlyphVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
    };

    virtual bool InitializeBuffers(ID3D11Device* device);

    bool UpdateVertexArray(const char* sentence,
                           const float renderX,
                           const float renderY);

    unsigned int m_screenWidth;
    unsigned int m_screenHeight;
    unsigned int m_maxLength;

    Font* m_font;
};