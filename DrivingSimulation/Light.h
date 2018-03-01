#pragma once

#include <DirectXMath.h>

class Light
{
public:
    Light();
    Light(const Light& other);
    ~Light();

    void SetAmbientColor(const float red,
                         const float green,
                         const float blue,
                         const float alpha);

    void SetDiffuseColor(const float red,
                         const float green,
                         const float blue,
                         const float alpha);

    void SetDirection(const float x,
                      const float y,
                      const float z);

    void SetSpecularColor(const float red,
                          const float green,
                          const float blue,
                          const float alpha);

    void SetSpecularPower(const float power);


    DirectX::XMFLOAT4 GetAmbientColor();
    DirectX::XMFLOAT4 GetDiffuseColor();
    DirectX::XMFLOAT3 GetDirection();
    DirectX::XMFLOAT4 GetSpecularColor();
    float GetSpecularPower();

private:
    DirectX::XMFLOAT4 m_ambientColor;
    DirectX::XMFLOAT4 m_diffuseColor;
    DirectX::XMFLOAT4 m_specularColor;
    DirectX::XMFLOAT3 m_direction;

    float m_specularPower;
};