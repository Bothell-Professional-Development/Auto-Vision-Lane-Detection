#pragma once

#include <d3d11.h>

class Texture
{
public:
    enum TextureImageFormat
    {
        TARGA = 0,
        UNKNOWN
    };

    struct TargaHeader
    {
        unsigned char idLength;
        unsigned char colorMapType;
        unsigned char imageType;
        unsigned char colorMapSpecification[5];
        unsigned short xOrigin;
        unsigned short yOrigin;
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char imageDescriptor;
    };

    Texture();
    Texture(const Texture& other);
    ~Texture();

    bool Initialize(ID3D11Device* device,
                    ID3D11DeviceContext* context,
                    const char* filename,
                    const TextureImageFormat imageFormat);

    bool WriteTextureToFile(ID3D11Device* device,
                            ID3D11DeviceContext* context,
                            const char* destinationFilename);

    bool WriteTextureToRaster(ID3D11Device* device,
                              ID3D11DeviceContext* context);

    ID3D11ShaderResourceView* GetTexture();
    const unsigned char* GetRasterData() const;

    const unsigned int& GetWidth() const;
    const unsigned int& GetHeight() const;
    const unsigned int GetRasterSize() const;

    virtual void Shutdown();

    static bool ConvertTargaBgrToRgb(const char* sourceFilename,
                                     const char* destinationFilename);
    static bool ConvertTargaColorToAlpha(const char* sourceFilename,
                                         const char* destinationFilename,
                                         const unsigned char red,
                                         const unsigned char green,
                                         const unsigned char blue);
protected:
    unsigned int m_width;
    unsigned int m_height;

    unsigned char* m_raster;

    ID3D11Texture2D* m_texture;
    ID3D11Texture2D* m_textureCopy;
    ID3D11ShaderResourceView* m_textureView;

private:
    bool LoadTargaTexture(const char* filename);
};