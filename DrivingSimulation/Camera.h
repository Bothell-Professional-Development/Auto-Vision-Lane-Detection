#pragma once

#include <DirectXMath.h>

class Camera
{
public:
    Camera();
    Camera(const Camera& other);
    ~Camera();

    //operator overloads fix alignment warning
    void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }

    void operator delete(void* p)
    {
        _mm_free(p);
    }

    void SetPosition(const float positionX,
                     const float positionY,
                     const float positionZ);

    void SetRotation(const float rotationX,
                     const float rotationY,
                     const float rotationZ);

    DirectX::XMFLOAT3 GetPosition() const;
    DirectX::XMFLOAT3 GetRotation() const;
    const DirectX::XMMATRIX& GetViewMatrix();
    const DirectX::XMMATRIX& GetBaseViewMatrix();

    void Render();
    void RenderBaseViewMatrix(const float positionX,
                              const float positionY,
                              const float positionZ,
                              const float rotationX,
                              const float rotationY,
                              const float rotationZ);

private:
    float m_positionX;
    float m_positionY;
    float m_positionZ;
    float m_rotationX;
    float m_rotationY;
    float m_rotationZ;
    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_baseViewMatrix;
};