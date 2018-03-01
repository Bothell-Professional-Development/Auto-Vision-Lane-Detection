#pragma once

#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>

class Font
{
public:
    struct Glyph
    {
        Glyph() :
            left(0.0f),
            right(0.0f),
            size(0){}

        Glyph(float l,
              float r,
              unsigned int s)
        {
            left = l;
            right = r;
            size = s;
        }

        float left;
        float right;
        unsigned int size;
    };

    Font();
    Font(const Font& other);
    ~Font();

    void Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context);

    void Shutdown();

    bool GetGlyph(const unsigned int character,
                  Font::Glyph& glyph);

private:

    void LoadFontData();

    std::vector<Glyph> m_glyphs;
};