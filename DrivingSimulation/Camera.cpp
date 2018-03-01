#include "stdafx.h"

#include "Camera.h"

Camera::Camera() :
    m_positionX(0.0f),
    m_positionY(0.0f),
    m_positionZ(0.0f),
    m_rotationX(0.0f),
    m_rotationY(0.0f),
    m_rotationZ(0.0f){}

Camera::Camera(const Camera& other){}
Camera::~Camera(){}

void Camera::SetPosition(const float positionX,
                         const float positionY,
                         const float positionZ)
{
    m_positionX = positionX;
    m_positionY = positionY;
    m_positionZ = positionZ;
}

void Camera::SetRotation(const float rotationX,
                         const float rotationY,
                         const float rotationZ)
{
    m_rotationX = rotationX;
    m_rotationY = rotationY;
    m_rotationZ = rotationZ;
}

DirectX::XMFLOAT3 Camera::GetPosition() const { return DirectX::XMFLOAT3(m_positionX, m_positionY, m_positionZ); }
DirectX::XMFLOAT3 Camera::GetRotation() const { return DirectX::XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ); }
const DirectX::XMMATRIX& Camera::GetViewMatrix(){ return m_viewMatrix; }
const DirectX::XMMATRIX& Camera::GetBaseViewMatrix(){ return m_baseViewMatrix; }

void Camera::Render()
{
    DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR upVector = DirectX::XMLoadFloat3(&up);

    DirectX::XMFLOAT3 position(m_positionX, m_positionY, m_positionZ);
    DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);

    DirectX::XMFLOAT3 lookAt(0.0f, 0.0f, 1.0f);
    DirectX::XMVECTOR lookAtVector = DirectX::XMLoadFloat3(&lookAt);

    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(m_rotationX,
                                                                       m_rotationY,
                                                                       m_rotationZ);
    
    lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotation);
    upVector = DirectX::XMVector3TransformCoord(upVector, rotation);

    lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

    m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void Camera::RenderBaseViewMatrix(const float positionX,
                                  const float positionY,
                                  const float positionZ,
                                  const float rotationX,
                                  const float rotationY,
                                  const float rotationZ)
{
    DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR upVector = DirectX::XMLoadFloat3(&up);

    DirectX::XMFLOAT3 position(positionX, positionY, positionZ);
    DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&position);

    DirectX::XMFLOAT3 lookAt(0.0f, 0.0f, 1.0f);
    DirectX::XMVECTOR lookAtVector = DirectX::XMLoadFloat3(&lookAt);

    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rotationX,
                                                                       rotationY,
                                                                       rotationZ);

    lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotation);
    upVector = DirectX::XMVector3TransformCoord(upVector, rotation);

    lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

    m_baseViewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}