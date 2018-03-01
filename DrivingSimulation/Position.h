#pragma once

#include <DirectXMath.h>

class Position
{
public:
    Position();
    Position(const Position& other);
    ~Position();

    void SetPosition(const float x,
                     const float y,
                     const float z);

    void SetRotation(const float x,
                     const float y,
                     const float z);

    const DirectX::XMFLOAT3& GetPosition() const;
    const DirectX::XMFLOAT3& GetRotation() const;

    void Frame(const float frameTime,
               const DirectX::XMFLOAT3 velocity,
               const DirectX::XMFLOAT3 rotationalVelocity);

private:
    void UpdatePosition(const float frameTime,
                        const DirectX::XMFLOAT3 velocity);

    void UpdateRotation(const float frameTime,
                        const DirectX::XMFLOAT3 rotationalVelocity);

    static const float DEFAULT_MAX_ROTATION_X;
    static const float DEFAULT_MIN_ROTATION_X;

    float m_maxRotationX;
    float m_minRotationX;

    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
};