#include "stdafx.h"

#include "Position.h"

const float Position::DEFAULT_MAX_ROTATION_X = DirectX::XMConvertToRadians(90.0f);
const float Position::DEFAULT_MIN_ROTATION_X = DirectX::XMConvertToRadians(-90.0f);;

Position::Position() :
    m_maxRotationX(DEFAULT_MAX_ROTATION_X),
    m_minRotationX(DEFAULT_MIN_ROTATION_X),
    m_position(0.0f, 0.0f, 0.0f),
    m_rotation(0.0f, 0.0f, 0.0f){}
Position::Position(const Position& other){}
Position::~Position(){}

void Position::SetPosition(const float x,
                           const float y,
                           const float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

void Position::SetRotation(const float x,
                           const float y,
                           const float z)
{
    m_rotation.x = x;

    if(m_rotation.x > m_maxRotationX)
    {
        m_rotation.x = m_maxRotationX;
    }
    else if(m_rotation.x < m_minRotationX)
    {
        m_rotation.x = m_minRotationX;
    }

    m_rotation.y = y;
    
    if(m_rotation.y > DirectX::XM_2PI)
    {
        m_rotation.y -= DirectX::XM_2PI;
    }
    else if(m_rotation.y < 0.0f)
    {
        m_rotation.y += DirectX::XM_2PI;
    }

    m_rotation.z = z;
}

const DirectX::XMFLOAT3& Position::GetPosition() const
{
    return m_position;
}

const DirectX::XMFLOAT3& Position::GetRotation() const 
{
    return m_rotation;
}

void Position::Frame(const float frameTime,
                     const DirectX::XMFLOAT3 velocity,
                     const DirectX::XMFLOAT3 rotationalVelocity)
{
    UpdatePosition(frameTime, velocity);
    UpdateRotation(frameTime, rotationalVelocity);
}

void Position::UpdatePosition(const float frameTime,
                              const DirectX::XMFLOAT3 velocity)
{
    float rotationY = m_rotation.y;
    float rotationYStrafe = rotationY + DirectX::XM_PIDIV2;
    SetPosition(m_position.x + sinf(rotationY) * velocity.z * frameTime +
                               sinf(rotationYStrafe) * velocity.x * frameTime,
                m_position.y,
                m_position.z + cosf(rotationY) * velocity.z * frameTime +
                               cosf(rotationYStrafe) * velocity.x * frameTime);
}

void Position::UpdateRotation(const float frameTime,
                              const DirectX::XMFLOAT3 rotationalVelocity)
{
    SetRotation(m_rotation.x + (rotationalVelocity.x * frameTime),
                m_rotation.y + (rotationalVelocity.y * frameTime),
                m_rotation.z);
}