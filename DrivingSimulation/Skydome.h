#pragma once

#include "Model.h"

class SkyDome : public Model
{
public:
    SkyDome();
    SkyDome(const SkyDome& other);
    ~SkyDome();

    bool Initialize(ID3D11Device* device,
                    const char* modelFilename,
                    const DirectX::XMFLOAT4 apexColor,
                    const DirectX::XMFLOAT4 centerColor);

    const DirectX::XMFLOAT4& GetApexColor() const;

private:
    DirectX::XMFLOAT4 m_apexColor;
};