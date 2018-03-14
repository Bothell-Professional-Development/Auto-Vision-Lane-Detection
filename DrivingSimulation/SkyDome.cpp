#include "stdafx.h"

#include "SkyDome.h"

SkyDome::SkyDome() :
    m_apexColor(0.0f, 0.0f, 0.0f, 0.0f){}
SkyDome::SkyDome(const SkyDome& other){}
SkyDome::~SkyDome(){}

bool SkyDome::Initialize(ID3D11Device* device,
                         const char* modelFilename,
                         const DirectX::XMFLOAT4 apexColor,
                         const DirectX::XMFLOAT4 centerColor)
{
    m_apexColor = apexColor;

    return Model::Initialize(device,
                             modelFilename,
                             centerColor);
}

const DirectX::XMFLOAT4& SkyDome::GetApexColor() const
{
    return m_apexColor;
}