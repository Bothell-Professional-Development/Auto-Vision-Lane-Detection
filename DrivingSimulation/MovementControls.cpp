#include "stdafx.h"

#include "MovementControls.h"
#include "System.h"

const bool MovementControls::DEFAULT_INVERT_MOUSE = false;

const unsigned char MovementControls::DEFAULT_FORWARD_KEY = DIK_W;
const unsigned char MovementControls::DEFAULT_BACKWARD_KEY = DIK_S;
const unsigned char MovementControls::DEFAULT_LEFT_KEY = DIK_LEFTARROW;
const unsigned char MovementControls::DEFAULT_RIGHT_KEY = DIK_RIGHTARROW;
const unsigned char MovementControls::DEFAULT_UP_KEY = DIK_UPARROW;
const unsigned char MovementControls::DEFAULT_DOWN_KEY = DIK_DOWNARROW;
const unsigned char MovementControls::DEFAULT_STRAFE_LEFT_KEY = DIK_A;
const unsigned char MovementControls::DEFAULT_STRAFE_RIGHT_KEY = DIK_D;

const float MovementControls::DEFAULT_FORWARD_ACCELERATION = 4.5f;
const float MovementControls::DEFAULT_BACKWARD_ACCELERATION = 3.0f;
const float MovementControls::DEFAULT_STRAFE_ACCELERATION = 4.5f;
const float MovementControls::DEFAULT_ORTHOGONAL_DECELERATION = 15.0f;

const float MovementControls::DEFAULT_MAX_FORWARD_VELOCITY = 6.0f;
const float MovementControls::DEFAULT_MAX_BACKWARD_VELOCITY = 4.0f;
const float MovementControls::DEFAULT_MAX_STRAFE_VELOCITY = 5.0f;

const float MovementControls::DEFAULT_MOUSE_SENSITIVITY_X = 0.20f;
const float MovementControls::DEFAULT_MOUSE_SENSITIVITY_Y = 0.20f;
const float MovementControls::DEFAULT_MOUSE_ACCELERATION_X = 0.0f;
const float MovementControls::DEFAULT_MOUSE_ACCELERATION_Y = 0.0f;

MovementControls::MovementControls() :
    m_input(NULL),
    m_invertMouse(DEFAULT_INVERT_MOUSE),
    m_forward(DEFAULT_FORWARD_KEY),
    m_backward(DEFAULT_BACKWARD_KEY),
    m_left(DEFAULT_LEFT_KEY),
    m_right(DEFAULT_RIGHT_KEY),
    m_up(DEFAULT_UP_KEY),
    m_down(DEFAULT_DOWN_KEY),
    m_strafeLeft(DEFAULT_STRAFE_LEFT_KEY),
    m_strafeRight(DEFAULT_STRAFE_RIGHT_KEY),
    m_mousePrevX(0),
    m_mousePrevY(0),
    m_frameTime(0.0f),
    m_forwardAcceleration(DEFAULT_FORWARD_ACCELERATION),
    m_backwardAcceleration(DEFAULT_BACKWARD_ACCELERATION),
    m_strafeAcceleration(DEFAULT_STRAFE_ACCELERATION),
    m_orthogonalDeceleration(DEFAULT_ORTHOGONAL_DECELERATION),
    m_maxForwardVelocity(DEFAULT_MAX_FORWARD_VELOCITY),
    m_maxBackwardVelocity(DEFAULT_MAX_BACKWARD_VELOCITY),
    m_maxStrafeVelocity(DEFAULT_MAX_STRAFE_VELOCITY),
    m_mouseSensitivityX(DEFAULT_MOUSE_SENSITIVITY_X),
    m_mouseSensitivityY(DEFAULT_MOUSE_SENSITIVITY_Y),
    m_mouseAccelerationX(DEFAULT_MOUSE_ACCELERATION_X),
    m_mouseAccelerationY(DEFAULT_MOUSE_ACCELERATION_Y){}
MovementControls::MovementControls(const MovementControls& other){}
MovementControls::~MovementControls(){}

bool MovementControls::Initialize(Input* input)
{
    bool success = true;

    if(input)
    {
        m_input = input;
    }
    else
    {
        System::GetInstance().ShowMessage(L"MovementControls::Initialize: Input device not valid",
                                          L"Error");
        success = false;
    }

    return success;
}

void MovementControls::SetForwardKey(const unsigned char forwardKey)
{
    m_forward = forwardKey;
}

void MovementControls::SetBackwardKey(const unsigned char backwardKey)
{
    m_backward = backwardKey;
}

void MovementControls::SetLeftKey(const unsigned char leftKey)
{
    m_left = leftKey;
}

void MovementControls::SetRightKey(const unsigned char rightKey)
{
    m_right = rightKey;
}

void MovementControls::SetMoveUpKey(const unsigned char upKey)
{
    m_up = upKey;
}

void MovementControls::SetMoveDownKey(const unsigned char downKey)
{
    m_down = downKey;
}

void MovementControls::SetStrafeLeftKey(const unsigned char strafeLeftKey)
{
    m_strafeLeft = strafeLeftKey;
}

void MovementControls::SetStrafeRightKey(const unsigned char strafeRightKey)
{
    m_strafeRight = strafeRightKey;
}

void MovementControls::SetForwardAcceleration(const float forwardAcceleration)
{
    m_forwardAcceleration = forwardAcceleration;
}

void MovementControls::SetBackwardAcceleration(const float backwardAcceleration)
{
    m_backwardAcceleration = backwardAcceleration;
}

void MovementControls::SetStrafeAcceleration(const float strafeAcceleration)
{
    m_strafeAcceleration = strafeAcceleration;
}

void MovementControls::SetOrthogonalDeceleration(const float deceleration)
{
    m_orthogonalDeceleration = deceleration;
}

void MovementControls::SetMaxForwardVelocity(const float maxForwardVelocity)
{
    m_maxForwardVelocity = maxForwardVelocity;
}

void MovementControls::SetMaxBackwardVelocity(const float maxBackwardVelocity)
{
    m_maxBackwardVelocity = maxBackwardVelocity;
}

void MovementControls::SetMaxStrafeVelocity(const float maxStrafeVelocity)
{
    m_maxStrafeVelocity = maxStrafeVelocity;
}

void MovementControls::SetMouseSensitivityX(const float sensitivity)
{
    m_mouseSensitivityX = sensitivity;
}

void MovementControls::SetMouseSensitivityY(const float sensitivity)
{
    m_mouseSensitivityY = sensitivity;
}

void MovementControls::SetMouseAccelerationX(const float acceleration)
{
    m_mouseAccelerationX = acceleration;
}

void MovementControls::SetMouseAccelerationY(const float acceleration)
{
    m_mouseAccelerationY = acceleration;
}

void MovementControls::SetVelocityX(const float velocityX)
{
    m_velocity.x = velocityX;

    if(m_velocity.x > m_maxStrafeVelocity)
    {
        m_velocity.x = m_maxStrafeVelocity;
    }
    else if(m_velocity.x < -m_maxStrafeVelocity)
    {
        m_velocity.x = -m_maxStrafeVelocity;
    }
}

void MovementControls::SetVelocityY(const float velocityY)
{
    m_velocity.y = velocityY;
}

void MovementControls::SetVelocityZ(const float velocityZ)
{
    m_velocity.z = velocityZ;

    if(m_velocity.z > m_maxForwardVelocity)
    {
        m_velocity.z = m_maxForwardVelocity;
    }
    else if(m_velocity.z < -m_maxBackwardVelocity)
    {
        m_velocity.z = -m_maxBackwardVelocity;
    }
}

const DirectX::XMFLOAT3& MovementControls::GetVelocity() const
{
    return m_velocity;
}

const DirectX::XMFLOAT3& MovementControls::GetRotationalVelocity() const
{
    return m_rotationalVelocity;
}

void MovementControls::Frame(const float frameTime)
{
    m_frameTime = frameTime;
    UpdateVelocity();
    UpdateRotationalVelocity();
}

void MovementControls::UpdateVelocity()
{
    UpdateVelocityX();
    UpdateVelocityY();
    UpdateVelocityZ();
}

void MovementControls::UpdateRotationalVelocity()
{
    UpdateRotationalVelocityXY();
    UpdateRotationalVelocityZ();
}

void MovementControls::UpdateVelocityX()
{
    if(m_input)
    {
        bool strafeLeftPressed = m_input->IsKeyPressed(m_strafeLeft);
        bool strafeRightPressed = m_input->IsKeyPressed(m_strafeRight);

        if(strafeLeftPressed && !strafeRightPressed)
        {
            SetVelocityX(m_velocity.x - (m_frameTime * m_strafeAcceleration));

            if(m_velocity.x > 0.0f)
            {
                SetVelocityX(m_velocity.x - (m_frameTime * m_orthogonalDeceleration));
            }
        }
        else if(strafeRightPressed && !strafeLeftPressed)
        {
            SetVelocityX(m_velocity.x + (m_frameTime * m_strafeAcceleration));

            if(m_velocity.x < 0.0f)
            {
                SetVelocityX(m_velocity.x + (m_frameTime * m_orthogonalDeceleration));
            }
        }
        else
        {
            if(m_velocity.x < 0.0f)
            {
                SetVelocityX(m_velocity.x + (m_frameTime * m_orthogonalDeceleration));

                //we don't want to rubberband as we slow down past 0
                if(m_velocity.x > 0.0f)
                {
                    SetVelocityX(0.0f);
                }
            }
            else if(m_velocity.x > 0.0f)
            {
                SetVelocityX(m_velocity.x - (m_frameTime * m_orthogonalDeceleration));

                //we don't want to rubberband as we slow down past 0
                if(m_velocity.x < 0.0f)
                {
                    SetVelocityX(0.0f);
                }
            }
        }
    }
}

void MovementControls::UpdateVelocityY()
{

}

void MovementControls::UpdateVelocityZ()
{
    if(m_input)
    {
        bool forwardPressed = m_input->IsKeyPressed(m_forward);
        bool backwardPressed = m_input->IsKeyPressed(m_backward);

        if(forwardPressed && !backwardPressed)
        {
            SetVelocityZ(m_velocity.z + (m_frameTime * m_forwardAcceleration));

            if(m_velocity.z < 0.0f)
            {
                SetVelocityZ(m_velocity.z + (m_frameTime * m_orthogonalDeceleration));
            }
        }
        else if(backwardPressed && !forwardPressed)
        {
            SetVelocityZ(m_velocity.z - (m_frameTime * m_backwardAcceleration));

            if(m_velocity.z > 0.0f)
            {
                SetVelocityZ(m_velocity.z - (m_frameTime * m_orthogonalDeceleration));
            }
        }
        else
        {
            if(m_velocity.z > 0.0f)
            {
                SetVelocityZ(m_velocity.z - (m_frameTime * m_orthogonalDeceleration));

                //we don't want to rubberband as we slow down past 0
                if(m_velocity.z < 0.0f)
                {
                    SetVelocityZ(0.0f);
                }
            }
            else if(m_velocity.z < 0.0f)
            {
                SetVelocityZ(m_velocity.z + (m_frameTime * m_orthogonalDeceleration));

                //we don't want to rubberband as we slow down past 0
                if(m_velocity.z > 0.0f)
                {
                    SetVelocityZ(0.0f);
                }
            }
        }
    }
}

void MovementControls::UpdateRotationalVelocityXY()
{
    if(m_input)
    {
        DirectX::XMUINT2 currentMouseLocation = m_input->GetMouseLocation();

        int deltaX = currentMouseLocation.x - m_mousePrevX;
        int deltaY = currentMouseLocation.y - m_mousePrevY;

        m_rotationalVelocity.x = (deltaY * m_mouseSensitivityY);

        //this acceleration implementation feels super weird, consider revising in future
        if(m_mouseAccelerationY > 0.0f)
        {
            m_rotationalVelocity.x = m_rotationalVelocity.x * (abs(deltaY) / (1.0f / m_mouseAccelerationY)) * m_frameTime;
        }

        if(m_invertMouse)
        {
            m_rotationalVelocity.x = -m_rotationalVelocity.x;
        }

        m_rotationalVelocity.y = (deltaX * m_mouseSensitivityX);

        //this acceleration implementation feels super weird, consider revising in future
        if(m_mouseAccelerationX > 0.0f)
        {
            m_rotationalVelocity.y = m_rotationalVelocity.y * (abs(deltaX) / (1.0f / m_mouseAccelerationX)) * m_frameTime;
        }

        m_mousePrevX = currentMouseLocation.x;
        m_mousePrevY = currentMouseLocation.y;
    }
}

void MovementControls::UpdateRotationalVelocityZ()
{

}