#include "stdafx.h"

#include "Light.h"

Light::Light(){}
Light::Light(const Light& other){}
Light::~Light(){}

void Light::SetAmbientColor(const float red,
                            const float green,
                            const float blue,
                            const float alpha)
{
    m_ambientColor = DirectX::XMFLOAT4(red, green, blue, alpha);
}

void Light::SetDiffuseColor(const float red,
                            const float green,
                            const float blue,
                            const float alpha)
{
    m_diffuseColor = DirectX::XMFLOAT4(red, green, blue, alpha);
}

void Light::SetDirection(const float x,
                         const float y,
                         const float z)
{
    m_direction = DirectX::XMFLOAT3(x, y, z);
}

void Light::SetSpecularColor(const float red,
                             const float green,
                             const float blue,
                             const float alpha)
{
    m_specularColor = DirectX::XMFLOAT4(red, green, blue, alpha);
}

void Light::SetSpecularPower(const float power)
{
    m_specularPower = power;
}

DirectX::XMFLOAT4 Light::GetAmbientColor()
{
    return m_ambientColor;
}

DirectX::XMFLOAT4 Light::GetDiffuseColor()
{
    return m_diffuseColor;
}

DirectX::XMFLOAT3 Light::GetDirection()
{
    return m_direction;
}

DirectX::XMFLOAT4 Light::GetSpecularColor()
{
    return m_specularColor;
}

float Light::GetSpecularPower()
{
    return m_specularPower;
}