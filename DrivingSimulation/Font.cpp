#include "stdafx.h"

#include "Font.h"

//This is a kind of hacky way of rendering text I found on rastertek
//Perhaps this can be improved upon by either using a vector font lib to 
//dynamically load glyph rasters. Alternatively, we may use a font lib, like
//Freetype, to generate the textures that this code uses. If the former is chosen,
//It would be a good idea to cache the raster results to minimize font rasterization
//operations.

Font::Font(){}
Font::Font(const Font& other){}
Font::~Font(){}

void Font::Initialize(ID3D11Device* device,
                      ID3D11DeviceContext* context)
{
    LoadFontData();
}

void Font::Shutdown()
{
    m_glyphs.clear();
}

bool Font::GetGlyph(const unsigned int character,
                    Font::Glyph& glyph)
{
    bool success = true;

    if(character - 32 < m_glyphs.size())
    {
        glyph = m_glyphs[character - 32];
    }
    else
    {
        success = false;
    }

    return success;
}

void Font::LoadFontData()
{
    m_glyphs.push_back(Glyph(0.0f, 0.0f, 3));
    m_glyphs.push_back(Glyph(0.0f, 0.000976563f, 1));
    m_glyphs.push_back(Glyph(0.00195313f, 0.00488281f, 3));
    m_glyphs.push_back(Glyph(0.00585938f, 0.0136719f, 8));
    m_glyphs.push_back(Glyph(0.0146484f, 0.0195313f, 5));
    m_glyphs.push_back(Glyph(0.0205078f, 0.0302734f, 10));
    m_glyphs.push_back(Glyph(0.03125f, 0.0390625f, 8));
    m_glyphs.push_back(Glyph(0.0400391f, 0.0410156f, 1));
    m_glyphs.push_back(Glyph(0.0419922f, 0.0449219f, 3));
    m_glyphs.push_back(Glyph(0.0458984f, 0.0488281f, 3));
    m_glyphs.push_back(Glyph(0.0498047f, 0.0546875f, 5));
    m_glyphs.push_back(Glyph(0.0556641f, 0.0625f, 7));
    m_glyphs.push_back(Glyph(0.0634766f, 0.0644531f, 1));
    m_glyphs.push_back(Glyph(0.0654297f, 0.0683594f, 3));
    m_glyphs.push_back(Glyph(0.0693359f, 0.0703125f, 1));
    m_glyphs.push_back(Glyph(0.0712891f, 0.0751953f, 4));
    m_glyphs.push_back(Glyph(0.0761719f, 0.0820313f, 6));
    m_glyphs.push_back(Glyph(0.0830078f, 0.0859375f, 3));
    m_glyphs.push_back(Glyph(0.0869141f, 0.0927734f, 6));
    m_glyphs.push_back(Glyph(0.09375f, 0.0996094f, 6));
    m_glyphs.push_back(Glyph(0.100586f, 0.106445f, 6));
    m_glyphs.push_back(Glyph(0.107422f, 0.113281f, 6));
    m_glyphs.push_back(Glyph(0.114258f, 0.120117f, 6));
    m_glyphs.push_back(Glyph(0.121094f, 0.126953f, 6));
    m_glyphs.push_back(Glyph(0.12793f, 0.133789f, 6));
    m_glyphs.push_back(Glyph(0.134766f, 0.140625f, 6));
    m_glyphs.push_back(Glyph(0.141602f, 0.142578f, 1));
    m_glyphs.push_back(Glyph(0.143555f, 0.144531f, 1));
    m_glyphs.push_back(Glyph(0.145508f, 0.151367f, 6));
    m_glyphs.push_back(Glyph(0.152344f, 0.15918f, 7));
    m_glyphs.push_back(Glyph(0.160156f, 0.166016f, 6));
    m_glyphs.push_back(Glyph(0.166992f, 0.171875f, 5));
    m_glyphs.push_back(Glyph(0.172852f, 0.18457f, 12));
    m_glyphs.push_back(Glyph(0.185547f, 0.194336f, 9));
    m_glyphs.push_back(Glyph(0.195313f, 0.202148f, 7));
    m_glyphs.push_back(Glyph(0.203125f, 0.209961f, 7));
    m_glyphs.push_back(Glyph(0.210938f, 0.217773f, 7));
    m_glyphs.push_back(Glyph(0.21875f, 0.225586f, 7));
    m_glyphs.push_back(Glyph(0.226563f, 0.232422f, 6));
    m_glyphs.push_back(Glyph(0.233398f, 0.241211f, 8));
    m_glyphs.push_back(Glyph(0.242188f, 0.249023f, 7));
    m_glyphs.push_back(Glyph(0.25f, 0.250977f, 1));
    m_glyphs.push_back(Glyph(0.251953f, 0.256836f, 5));
    m_glyphs.push_back(Glyph(0.257813f, 0.265625f, 8));
    m_glyphs.push_back(Glyph(0.266602f, 0.272461f, 6));
    m_glyphs.push_back(Glyph(0.273438f, 0.282227f, 9));
    m_glyphs.push_back(Glyph(0.283203f, 0.290039f, 7));
    m_glyphs.push_back(Glyph(0.291016f, 0.298828f, 8));
    m_glyphs.push_back(Glyph(0.299805f, 0.306641f, 7));
    m_glyphs.push_back(Glyph(0.307617f, 0.31543f, 8));
    m_glyphs.push_back(Glyph(0.316406f, 0.323242f, 7));
    m_glyphs.push_back(Glyph(0.324219f, 0.331055f, 7));
    m_glyphs.push_back(Glyph(0.332031f, 0.338867f, 7));
    m_glyphs.push_back(Glyph(0.339844f, 0.34668f, 7));
    m_glyphs.push_back(Glyph(0.347656f, 0.356445f, 9));
    m_glyphs.push_back(Glyph(0.357422f, 0.370117f, 13));
    m_glyphs.push_back(Glyph(0.371094f, 0.37793f, 7));
    m_glyphs.push_back(Glyph(0.378906f, 0.385742f, 7));
    m_glyphs.push_back(Glyph(0.386719f, 0.393555f, 7));
    m_glyphs.push_back(Glyph(0.394531f, 0.396484f, 2));
    m_glyphs.push_back(Glyph(0.397461f, 0.401367f, 4));
    m_glyphs.push_back(Glyph(0.402344f, 0.404297f, 2));
    m_glyphs.push_back(Glyph(0.405273f, 0.410156f, 5));
    m_glyphs.push_back(Glyph(0.411133f, 0.417969f, 7));
    m_glyphs.push_back(Glyph(0.418945f, 0.420898f, 2));
    m_glyphs.push_back(Glyph(0.421875f, 0.426758f, 5));
    m_glyphs.push_back(Glyph(0.427734f, 0.432617f, 5));
    m_glyphs.push_back(Glyph(0.433594f, 0.438477f, 5));
    m_glyphs.push_back(Glyph(0.439453f, 0.444336f, 5));
    m_glyphs.push_back(Glyph(0.445313f, 0.450195f, 5));
    m_glyphs.push_back(Glyph(0.451172f, 0.455078f, 4));
    m_glyphs.push_back(Glyph(0.456055f, 0.460938f, 5));
    m_glyphs.push_back(Glyph(0.461914f, 0.466797f, 5));
    m_glyphs.push_back(Glyph(0.467773f, 0.46875f, 1));
    m_glyphs.push_back(Glyph(0.469727f, 0.472656f, 3));
    m_glyphs.push_back(Glyph(0.473633f, 0.478516f, 5));
    m_glyphs.push_back(Glyph(0.479492f, 0.480469f, 1));
    m_glyphs.push_back(Glyph(0.481445f, 0.490234f, 9));
    m_glyphs.push_back(Glyph(0.491211f, 0.496094f, 5));
    m_glyphs.push_back(Glyph(0.49707f, 0.501953f, 5));
    m_glyphs.push_back(Glyph(0.50293f, 0.507813f, 5));
    m_glyphs.push_back(Glyph(0.508789f, 0.513672f, 5));
    m_glyphs.push_back(Glyph(0.514648f, 0.517578f, 3));
    m_glyphs.push_back(Glyph(0.518555f, 0.523438f, 5));
    m_glyphs.push_back(Glyph(0.524414f, 0.527344f, 3));
    m_glyphs.push_back(Glyph(0.52832f, 0.533203f, 5));
    m_glyphs.push_back(Glyph(0.53418f, 0.539063f, 5));
    m_glyphs.push_back(Glyph(0.540039f, 0.548828f, 9));
    m_glyphs.push_back(Glyph(0.549805f, 0.554688f, 5));
    m_glyphs.push_back(Glyph(0.555664f, 0.560547f, 5));
    m_glyphs.push_back(Glyph(0.561523f, 0.566406f, 5));
    m_glyphs.push_back(Glyph(0.567383f, 0.570313f, 3));
    m_glyphs.push_back(Glyph(0.571289f, 0.572266f, 1));
    m_glyphs.push_back(Glyph(0.573242f, 0.576172f, 3));
    m_glyphs.push_back(Glyph(0.577148f, 0.583984f, 7));
}