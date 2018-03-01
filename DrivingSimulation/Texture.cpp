#include "stdafx.h"

#include "Texture.h"
#include "System.h"

Texture::Texture() :
    m_width(0),
    m_height(0),
    m_raster(NULL),
    m_texture(NULL),
    m_textureView(NULL){}
Texture::Texture(const Texture& other){}
Texture::~Texture(){}

bool Texture::Initialize(ID3D11Device* device,
                         ID3D11DeviceContext* context,
                         const char* filename,
                         const TextureImageFormat imageFormat)
{
    bool success = true;

    switch(imageFormat)
    {
        case TARGA:
            success = LoadTargaTexture(filename);
            break;
        default:
            success = false;
            break;
    }

    if(success)
    {
        D3D11_TEXTURE2D_DESC textureDesc;

        textureDesc.Width = m_width;
        textureDesc.Height = m_height;
        textureDesc.MipLevels = 0;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        if(FAILED(device->CreateTexture2D(&textureDesc,
                                          NULL,
                                          &m_texture)))
        {
            System::GetInstance().ShowMessage(L"Texture::Initialize: Failed to create texture",
                                              L"Error");
            success = false;
        }
        else
        {
            context->UpdateSubresource(m_texture,
                                       0,
                                       NULL,
                                       m_raster,
                                       m_width * 4,
                                       0);

            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

            shaderResourceViewDesc.Format = textureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = -1;

            if(FAILED(device->CreateShaderResourceView(m_texture,
                                                       &shaderResourceViewDesc,
                                                       &m_textureView)))
            {
                System::GetInstance().ShowMessage(L"Texture::Initialize: Failed to create shader resource view",
                                                  L"Error");
                success = false;
            }
            else
            {
                context->GenerateMips(m_textureView);
            }
        }
    }

    return success;
}

bool Texture::WriteTextureToFile(ID3D11Device* device,
                                 ID3D11DeviceContext* context,
                                 const char* destinationFilename)
{
    bool success = true;

    if(m_raster)
    {
        TargaHeader header;

        header.idLength = 0;
        header.colorMapType = 0;
        header.imageType = 2;
        header.colorMapSpecification[0] = 0;
        header.colorMapSpecification[1] = 0;
        header.colorMapSpecification[2] = 0;
        header.colorMapSpecification[3] = 0;
        header.colorMapSpecification[4] = 0;
        header.xOrigin = 0;
        header.yOrigin = 0;
        header.width = m_width;
        header.height = m_height;
        header.bpp = 32;
        header.imageDescriptor = 0;

        std::ofstream  dst(destinationFilename, std::ios::binary);

        if(dst.is_open())
        {
            dst << header.idLength;
            dst << header.colorMapType;
            dst << header.imageType;

            for(unsigned int i = 0; i < 5; ++i)
            {
                dst << header.colorMapSpecification[i];
            }

            dst << static_cast<unsigned char>(header.xOrigin & 0xff);
            dst << static_cast<unsigned char>(header.xOrigin >> 8 & 0xff);
            dst << static_cast<unsigned char>(header.yOrigin & 0xff);
            dst << static_cast<unsigned char>(header.yOrigin >> 8 & 0xff);

            dst << static_cast<unsigned char>(header.width & 0xff);
            dst << static_cast<unsigned char>(header.width >> 8 & 0xff);
            dst << static_cast<unsigned char>(header.height & 0xff);
            dst << static_cast<unsigned char>(header.height >> 8 & 0xff);
            dst << header.bpp;
            dst << header.imageDescriptor;

            for(int i = m_height - 1; i >= 0; --i)
            {
                int rowStart = i * m_width * 4;

                for(unsigned int j = 0; j < m_width * 4; j += 4)
                {
                    dst << m_raster[rowStart + j+ 2];
                    dst << m_raster[rowStart + j + 1];
                    dst << m_raster[rowStart + j];
                    dst << m_raster[rowStart + j + 3];
                }
            }

            dst.close();
        }
        else
        {
            System::GetInstance().ShowMessage(L"Texture::WriteTextureToFile: Unable to open output file",
                                                L"Error");
            success = false;
        }
    }
    else
    {
        System::GetInstance().ShowMessage(L"Texture::WriteTextureToFile: No raster data to write",
                                          L"Error");
        success = false;
    }

    return success;
}

bool Texture::WriteTextureToRaster(ID3D11Device* device,
                                   ID3D11DeviceContext* context)
{
    bool success = true;

    if(m_texture)
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        m_texture->GetDesc(&textureDesc);

        textureDesc.BindFlags = 0;

        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        textureDesc.Usage = D3D11_USAGE_STAGING;

        unsigned int rasterSize = m_width * m_height * 4;

        ID3D11Texture2D* textureCopy = NULL;

        if(SUCCEEDED(device->CreateTexture2D(&textureDesc,
                                             NULL,
                                             &textureCopy)))
        {
            context->CopyResource(textureCopy, m_texture);
            
            D3D11_MAPPED_SUBRESOURCE mappedData;
            unsigned int subResource = D3D11CalcSubresource(0, 0, 0);

            if(SUCCEEDED(context->Map(textureCopy,
                                      subResource,
                                      D3D11_MAP_READ,
                                      0,
                                      &mappedData)))
            {
                //IS THIS RIGHT?
                for(unsigned int i = 0; i < rasterSize; ++i)
                {
                    m_raster[i] = static_cast<unsigned char>(static_cast<float*>(mappedData.pData)[i] * 256);
                }

                context->Unmap(textureCopy, subResource);
            }
            else
            {
                System::GetInstance().ShowMessage(L"Texture::WriteTextureToRaster: Unable to map texture resource",
                                                    L"Error");
                success = false;
            }
        }
        else
        {
            System::GetInstance().ShowMessage(L"Texture::WriteTextureToRaster: Unable copy texture resource",
                                              L"Error");
            success = false;
        }

        if(textureCopy)
        {
            textureCopy->Release();
        }
    }
    else
    {
        System::GetInstance().ShowMessage(L"Texture::WriteTextureToRaster: Nothing to write, m_texture is NULL",
                                            L"Error");
        success = false;
    }
        
    return success;
}

void Texture::Shutdown()
{
    if(m_raster)
    {
        delete[] m_raster;
        m_raster = NULL;
    }

    if(m_texture)
    {
        m_texture->Release();
        m_texture = NULL;
    }

    if(m_textureView)
    {
        m_textureView->Release();
        m_textureView = NULL;
    }
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
    return m_textureView;
}

const unsigned char* Texture::GetRasterData() const
{
    return m_raster;
}

const unsigned int& Texture::GetWidth() const 
{
    return m_width;
}

const unsigned int& Texture::GetHeight() const
{
    return m_height;
}

const unsigned int Texture::GetRasterSize() const
{
    return m_width * m_height * 4;
}

bool Texture::ConvertTargaBgrToRgb(const char* sourceFilename,
                                   const char* destinationFilename)
{
    bool success = true;

    FILE* src = NULL;
    
    std::ofstream  dst(destinationFilename, std::ios::binary);

    if(fopen_s(&src, sourceFilename, "rb") != 0)
    {
        System::GetInstance().ShowMessage(L"Texture::ConvertTargaBgrToRgb: Failed to load TGA texture image",
                                          L"Error");
        success = false;
    }
    else
    {
        TargaHeader targaHeader;
        size_t readSize = fread(&targaHeader, sizeof(TargaHeader), 1, src);
        unsigned char depth = targaHeader.bpp / 8;

        if(readSize != 1)
        {
            System::GetInstance().ShowMessage(L"Texture::ConvertTargaBgrToRgb: Failed to read TGA header",
                                              L"Error");
            success = false;
        }
        else if(depth != 3 && depth != 4)
        {
            System::GetInstance().ShowMessage(L"Texture::ConvertTargaBgrToRgb: TGA color depth unsupported",
                                              L"Error");
            success = false;
        }

        else
        {
            dst << targaHeader.idLength;
            dst << targaHeader.colorMapType;
            dst << targaHeader.imageType;

            for(unsigned int i = 0; i < 5; ++i)
            {
                dst << targaHeader.colorMapSpecification[i];
            }

            dst << static_cast<unsigned char>(targaHeader.xOrigin & 0xff);
            dst << static_cast<unsigned char>(targaHeader.xOrigin >> 8 & 0xff);
            dst << static_cast<unsigned char>(targaHeader.yOrigin & 0xff);
            dst << static_cast<unsigned char>(targaHeader.yOrigin >> 8 & 0xff);

            dst << static_cast<unsigned char>(targaHeader.width & 0xff);
            dst << static_cast<unsigned char>(targaHeader.width >> 8 & 0xff);
            dst << static_cast<unsigned char>(targaHeader.height & 0xff);
            dst << static_cast<unsigned char>(targaHeader.height >> 8 & 0xff);
            dst << targaHeader.bpp;
            dst << targaHeader.imageDescriptor;

            unsigned int imageSize = targaHeader.width * targaHeader.height * depth;
            unsigned char* data = new unsigned char[imageSize];

            readSize = fread(data, 1, imageSize, src);

            if(readSize != imageSize)
            {
                System::GetInstance().ShowMessage(L"Texture::ConvertTargaBgrToRgb: Error occurred while reading image data",
                                                  L"Error");
                success = false;
            }
            else
            {
                for(unsigned int i = 0; i < imageSize; i += depth)
                {
                    dst << data[i + 2];
                    dst << data[i + 1];
                    dst << data[i];

                    if(depth == 4)
                    {
                        dst << data[i + 3];
                    }
                }
            }
        }
    }

    if(src)
    {
        if(fclose(src) != 0)
        {
            System::GetInstance().ShowMessage(L"Texture::ConvertTargaBgrToRgb: Could not properly close source TGA file",
                                              L"Error");
            success = false;
        }
    }
    
    if(dst.is_open())
    {
        dst.close();
    }

    return success;
}

bool Texture::ConvertTargaColorToAlpha(const char* sourceFilename,
                                       const char* destinationFilename,
                                       const unsigned char red,
                                       const unsigned char green,
                                       const unsigned char blue)
{
    bool success = true;

    FILE* src = NULL;

    std::ofstream  dst(destinationFilename, std::ios::binary);

    if(fopen_s(&src, sourceFilename, "rb") != 0)
    {
        System::GetInstance().ShowMessage(L"Texture::ConvertTargaColorToAlpha: Failed to load TGA texture image",
                                          L"Error");
        success = false;
    }
    else
    {
        TargaHeader targaHeader;
        size_t readSize = fread(&targaHeader, sizeof(TargaHeader), 1, src);
        unsigned char depth = targaHeader.bpp / 8;

        if(readSize != 1)
        {
            System::GetInstance().ShowMessage(L"Texture::ConvertTargaColorToAlpha: Failed to read TGA header",
                                              L"Error");
            success = false;
        }
        else if(depth != 3 && depth != 4)
        {
            System::GetInstance().ShowMessage(L"Texture::ConvertTargaColorToAlpha: TGA color depth unsupported",
                                              L"Error");
            success = false;
        }

        else
        {
            dst << targaHeader.idLength;
            dst << targaHeader.colorMapType;
            dst << targaHeader.imageType;

            for(unsigned int i = 0; i < 5; ++i)
            {
                dst << targaHeader.colorMapSpecification[i];
            }

            dst << static_cast<unsigned char>(targaHeader.xOrigin & 0xff);
            dst << static_cast<unsigned char>(targaHeader.xOrigin >> 8 & 0xff);
            dst << static_cast<unsigned char>(targaHeader.yOrigin & 0xff);
            dst << static_cast<unsigned char>(targaHeader.yOrigin >> 8 & 0xff);

            dst << static_cast<unsigned char>(targaHeader.width & 0xff);
            dst << static_cast<unsigned char>(targaHeader.width >> 8 & 0xff);
            dst << static_cast<unsigned char>(targaHeader.height & 0xff);
            dst << static_cast<unsigned char>(targaHeader.height >> 8 & 0xff);
            dst << static_cast<unsigned char>(32);
            dst << static_cast<unsigned char>(0);

            unsigned int imageSize = targaHeader.width * targaHeader.height * depth;
            unsigned char* data = new unsigned char[imageSize];

            readSize = fread(data, 1, imageSize, src);

            if(readSize != imageSize)
            {
                System::GetInstance().ShowMessage(L"Texture::ConvertTargaColorToAlpha: Error occurred while reading image data",
                                                  L"Error");
                success = false;
            }
            else
            {
                int t = 0;
                for(unsigned int i = 0; i < imageSize; i += depth)
                {

                    //dst << static_cast<unsigned char>(0xFF);

                    if(data[i] == red &&
                       data[i + 1] == green &&
                       data[i + 2] == blue)
                    {
                        dst << static_cast<unsigned char>(0x0);
                        dst << static_cast<unsigned char>(0x0);
                        dst << static_cast<unsigned char>(0x0);
                        dst << static_cast<unsigned char>(0x0);
                    }
                    else
                    {
                        dst << data[i];
                        dst << data[i + 1];
                        dst << data[i + 2];

                        if(depth == 4)
                        {
                            dst << data[i + 3];
                        }
                        else
                        {
                            dst << static_cast<unsigned char>(0xFF);
                        }
                    }
                    t += 4;
                }

                t;
                t = 9;
            }
        }
    }

    if(src)
    {
        if(fclose(src) != 0)
        {
            System::GetInstance().ShowMessage(L"Texture::ConvertColorToAlpha: Could not properly close source TGA file",
                                              L"Error");
            success = false;
        }
    }

    if(dst.is_open())
    {
        dst.close();
    }

    return success;
}

bool Texture::LoadTargaTexture(const char* filename)
{
    //in case texture was previously loaded and not shut down
    if(m_raster)
    {
        delete[] m_raster;
        m_raster = NULL;
    }

    bool success = true;
    
    FILE* file = NULL;
    size_t readSize;
    unsigned int depth;

    if(fopen_s(&file, filename, "rb") != 0)
    {
        System::GetInstance().ShowMessage(L"Texture::LoadTargaTexture: Failed to load TGA texture image",
                                          L"Error");
        success = false;
    }
    else
    {
        TargaHeader targaHeader;
        readSize = fread(&targaHeader, sizeof(TargaHeader), 1, file);

        if(readSize != 1)
        {
            System::GetInstance().ShowMessage(L"Texture::LoadTargaTexture: Failed to read TGA header",
                                              L"Error");
            success = false;
        }
        else
        {
            m_width = targaHeader.width;
            m_height = targaHeader.height;
            depth = targaHeader.bpp / 8;

            if(depth != 3 && depth != 4)
            {
                System::GetInstance().ShowMessage(L"Texture::LoadTargaTexture: TGA color depth unsupported",
                                                  L"Error");
                success = false;
            }
        }
    }

    if(success)
    {
        unsigned int imageSize = m_width * m_height * depth;
        unsigned char* data = new unsigned char[imageSize];

        //we are always dealing with a 32bpp output texturemap
        m_raster = new unsigned char[m_width * m_height * 4];

        readSize = fread(data, 1, imageSize, file);

        if(readSize != imageSize)
        {
            System::GetInstance().ShowMessage(L"Texture::LoadTargaTexture: Error occurred while reading image data",
                                              L"Error");
            success = false;
        }
        else
        {
            unsigned int writeIndex = 0;
            unsigned int readIndex = (m_width * m_height * depth) - (m_width * depth);

            for(unsigned int i = 0; i < m_height; ++i)
            {
                for(unsigned int j = 0; j < m_width; ++j)
                {
                    m_raster[writeIndex] = data[readIndex + 2];
                    m_raster[writeIndex + 1] = data[readIndex + 1];
                    m_raster[writeIndex + 2] = data[readIndex];

                    if(depth == 4)
                    {
                        m_raster[writeIndex + 3] = data[readIndex + 3];
                    }
                    else
                    {
                        //if no alpha channel; make opaque
                        m_raster[writeIndex + 3] = 0xFF;
                    }

                    writeIndex += 4;
                    readIndex += depth;
                }

                readIndex -= (m_width * (depth == 4 ? 8 : 6));
            }
        }

        delete[] data;
        data = NULL;
    }

    if(file)
    {
        if(fclose(file) != 0)
        {
            System::GetInstance().ShowMessage(L"Texture::LoadTargaTexture: Could not properly close TGA file",
                                              L"Error");
            success = false;
        }
    }

    return success;
}