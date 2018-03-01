#pragma once

#include <DirectXMath.h>

#include "Input.h"

class MovementControls
{
public:
    MovementControls();
    MovementControls(const MovementControls& other);
    ~MovementControls();

    bool Initialize(Input* input);

    void SetForwardKey(const unsigned char forwardKey);
    void SetBackwardKey(const unsigned char backwardKey);
    void SetLeftKey(const unsigned char leftKey);
    void SetRightKey(const unsigned char rightKey);
    void SetMoveUpKey(const unsigned char upKey);
    void SetMoveDownKey(const unsigned char downKey);
    void SetStrafeLeftKey(const unsigned char strafeLeftKey);
    void SetStrafeRightKey(const unsigned char strafeRightKey);

    void SetForwardAcceleration(const float forwardAcceleration);
    void SetBackwardAcceleration(const float backwardAcceleration);
    void SetStrafeAcceleration(const float strafeAcceleration);
    void SetOrthogonalDeceleration(const float deceleration);

    void SetMaxForwardVelocity(const float maxForwardVelocity);
    void SetMaxBackwardVelocity(const float maxBackwardVelocity);
    void SetMaxStrafeVelocity(const float maxStrafeVelocity);

    void SetMouseSensitivityX(const float sensitivity);
    void SetMouseSensitivityY(const float sensitivity);
    void SetMouseAccelerationX(const float acceleration);
    void SetMouseAccelerationY(const float acceleration);

    void SetVelocityX(const float velocityX);
    void SetVelocityY(const float velocityY);
    void SetVelocityZ(const float velocityZ);

    const DirectX::XMFLOAT3& GetVelocity() const;
    const DirectX::XMFLOAT3& GetRotationalVelocity() const;

    void Frame(const float frameTime);

protected:
    void UpdateVelocity();
    void UpdateRotationalVelocity();
    virtual void UpdateVelocityX();
    virtual void UpdateVelocityY();
    virtual void UpdateVelocityZ();
    virtual void UpdateRotationalVelocityXY();
    virtual void UpdateRotationalVelocityZ();
    
    static const bool DEFAULT_INVERT_MOUSE;

    static const unsigned char DEFAULT_FORWARD_KEY;
    static const unsigned char DEFAULT_BACKWARD_KEY;
    static const unsigned char DEFAULT_LEFT_KEY;
    static const unsigned char DEFAULT_RIGHT_KEY;
    static const unsigned char DEFAULT_UP_KEY;
    static const unsigned char DEFAULT_DOWN_KEY;
    static const unsigned char DEFAULT_STRAFE_LEFT_KEY;
    static const unsigned char DEFAULT_STRAFE_RIGHT_KEY;

    static const float DEFAULT_FORWARD_ACCELERATION;
    static const float DEFAULT_BACKWARD_ACCELERATION;
    static const float DEFAULT_STRAFE_ACCELERATION;
    static const float DEFAULT_ORTHOGONAL_DECELERATION;

    static const float DEFAULT_MAX_FORWARD_VELOCITY;
    static const float DEFAULT_MAX_BACKWARD_VELOCITY;
    static const float DEFAULT_MAX_STRAFE_VELOCITY;

    static const float DEFAULT_MOUSE_SENSITIVITY_X;
    static const float DEFAULT_MOUSE_SENSITIVITY_Y;
    static const float DEFAULT_MOUSE_ACCELERATION_X;
    static const float DEFAULT_MOUSE_ACCELERATION_Y;
    
    Input* m_input;

    bool m_invertMouse;
    
    unsigned char m_forward;
    unsigned char m_backward;
    unsigned char m_left;
    unsigned char m_right;
    unsigned char m_up;
    unsigned char m_down;
    unsigned char m_strafeLeft;
    unsigned char m_strafeRight;

    unsigned int m_mousePrevX;
    unsigned int m_mousePrevY;

    float m_frameTime;

    float m_forwardAcceleration;
    float m_backwardAcceleration;
    float m_strafeAcceleration;
    float m_orthogonalDeceleration;

    float m_maxForwardVelocity;
    float m_maxBackwardVelocity;
    float m_maxStrafeVelocity;

    float m_mouseSensitivityX;
    float m_mouseSensitivityY;
    float m_mouseAccelerationX;
    float m_mouseAccelerationY;

    DirectX::XMFLOAT3 m_velocity;
    DirectX::XMFLOAT3 m_rotationalVelocity;
};