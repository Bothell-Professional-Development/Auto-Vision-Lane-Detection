#include "stdafx.h"

#include "DrivingMovementControls.h"

const float DrivingMovementControls::DEFAULT_MAX_STEERING_POSITION = DirectX::XM_2PI;
const float DrivingMovementControls::DEFAULT_MAX_STEERING_ANGULAR_VELOCITY = DirectX::XM_PI;
const float DrivingMovementControls::DEFAULT_TURNING_RADIUS = 7.0f;

DrivingMovementControls::DrivingMovementControls() :
    m_acceleratorPosition(0.0f),
    m_desiredSteeringPosition(0.0f),
    m_steeringPosition(0.0f),
    m_maxSteeringPosition(DEFAULT_MAX_STEERING_POSITION),
    m_maxSteeringAngularVelocity(DEFAULT_MAX_STEERING_ANGULAR_VELOCITY),
    m_turningRadius(DEFAULT_TURNING_RADIUS){}

DrivingMovementControls::DrivingMovementControls(const DrivingMovementControls& other){}
DrivingMovementControls::~DrivingMovementControls(){}

void DrivingMovementControls::SetAcceleratorPosition(const float acceleratorPosition)
{
    m_acceleratorPosition = acceleratorPosition;

    if(m_acceleratorPosition > 1.0f)
    {
        m_acceleratorPosition = 1.0f;
    }
    else if(m_acceleratorPosition < 0.0f)
    {
        m_acceleratorPosition = 0.0f;
    }
}

void DrivingMovementControls::SetDesiredSteeringPosition(const float steeringPosition)
{
    m_desiredSteeringPosition = steeringPosition;

    if(m_desiredSteeringPosition > m_maxSteeringPosition)
    {
        m_desiredSteeringPosition = m_maxSteeringPosition;
    }
    else if(m_desiredSteeringPosition < -m_maxSteeringPosition)
    {
        m_desiredSteeringPosition = -m_maxSteeringPosition;
    }
}

void DrivingMovementControls::SetSteeringPosition(const float steeringPosition)
{
    m_steeringPosition = steeringPosition;

    if(m_steeringPosition > m_maxSteeringPosition)
    {
        m_steeringPosition = m_maxSteeringPosition;
    }
    else if(m_steeringPosition < -m_maxSteeringPosition)
    {
        m_steeringPosition = -m_maxSteeringPosition;
    }
}

void DrivingMovementControls::SetMaxSteeringPosition(const float maxSteeringPosition)
{
    m_maxSteeringPosition = maxSteeringPosition;
}

void DrivingMovementControls::SetMaxSteeringAngularVelocity(const float angularVelocity)
{
    m_maxSteeringAngularVelocity = angularVelocity;
}

void DrivingMovementControls::SetTurningRadius(const float turningRadius)
{
    m_turningRadius = turningRadius;
}

const float& DrivingMovementControls::GetAcceleratorPosition() const
{
    return m_acceleratorPosition;
}

const float& DrivingMovementControls::GetDesiredSteeringPosition() const
{
    return m_desiredSteeringPosition;
}

const float& DrivingMovementControls::GetSteeringPosition() const
{
    return m_steeringPosition;
}

const float& DrivingMovementControls::GetMaxSteeringPosition() const
{
    return m_maxSteeringPosition;
}

const float& DrivingMovementControls::GetMaxSteeringAngularVelocity() const
{
    return m_maxSteeringAngularVelocity;
}

const float& DrivingMovementControls::GetTurningRadius() const
{
    return m_turningRadius;
}

void DrivingMovementControls::UpdateVelocityX(){}
void DrivingMovementControls::UpdateVelocityY(){}

void DrivingMovementControls::UpdateVelocityZ()
{
    if(m_input)
    {
        bool forwardKeyPressed = m_input->IsKeyPressed(m_forward);
        bool backwardKeyPressed = m_input->IsKeyPressed(m_backward);

        if(forwardKeyPressed && !backwardKeyPressed)
        {
            SetAcceleratorPosition(m_acceleratorPosition + 0.05f);
        }
        else if(backwardKeyPressed && !forwardKeyPressed)
        {
            SetAcceleratorPosition(m_acceleratorPosition - 0.05f);
        }

        float velocityZ = m_velocity.z;

        if(m_velocity.z < m_acceleratorPosition * m_maxForwardVelocity)
        {
            velocityZ += m_forwardAcceleration * m_frameTime;

            if(velocityZ > m_acceleratorPosition * m_maxForwardVelocity)
            {
                velocityZ = m_acceleratorPosition * m_maxForwardVelocity;
            }
        }
        else if(m_velocity.z > m_acceleratorPosition * m_maxForwardVelocity)
        {
            velocityZ -= m_orthogonalDeceleration * m_frameTime;

            if(velocityZ < 0.0f)
            {
                velocityZ = 0.0f;
            }
        }

        MovementControls::SetVelocityZ(velocityZ);
    }    
}

void DrivingMovementControls::UpdateRotationalVelocityXY()
{
    if(m_input)
    {
        bool leftKeyPressed = m_input->IsKeyPressed(m_left);
        bool rightKeyPressed = m_input->IsKeyPressed(m_right);

        if(leftKeyPressed && !rightKeyPressed)
        {
            SetDesiredSteeringPosition(m_desiredSteeringPosition - DirectX::XMConvertToRadians(2.5f));
        }
        else if(rightKeyPressed && !leftKeyPressed)
        {
            SetDesiredSteeringPosition(m_desiredSteeringPosition + DirectX::XMConvertToRadians(2.5f));
        }

        if(m_steeringPosition < m_desiredSteeringPosition)
        {
            float adjustedPosition = m_steeringPosition + m_maxSteeringAngularVelocity * m_frameTime;

            if(adjustedPosition > m_desiredSteeringPosition)
            {
                adjustedPosition = m_desiredSteeringPosition;
            }

            SetSteeringPosition(adjustedPosition);
        }
        else if(m_steeringPosition > m_desiredSteeringPosition)
        {
            float adjustedPosition = m_steeringPosition - m_maxSteeringAngularVelocity * m_frameTime;

            if(adjustedPosition < m_desiredSteeringPosition)
            {
                adjustedPosition = m_desiredSteeringPosition;
            }

            SetSteeringPosition(adjustedPosition);
        }

        float circumference = (m_maxSteeringPosition / m_steeringPosition) * m_turningRadius * DirectX::XM_2PI;

        if(circumference != 0.0f)
        {
            m_rotationalVelocity.y = (m_velocity.z / circumference) * DirectX::XM_2PI;
        }
    }
}

void DrivingMovementControls::UpdateRotationalVelocityZ(){}