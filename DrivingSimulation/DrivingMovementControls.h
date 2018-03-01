#pragma once

#include "MovementControls.h"

class DrivingMovementControls : public MovementControls
{
public:
    DrivingMovementControls();
    DrivingMovementControls(const DrivingMovementControls& other);
    ~DrivingMovementControls();

    void SetAcceleratorPosition(const float acceleratorPosition);
    void SetDesiredSteeringPosition(const float steeringPosition);
    void SetSteeringPosition(const float steeringPosition);
    void SetMaxSteeringPosition(const float maxSteeringPosition);
    void SetMaxSteeringAngularVelocity(const float angularVelocity);
    void SetTurningRadius(const float turningRadius);

    const float& GetAcceleratorPosition() const;
    const float& GetDesiredSteeringPosition() const;
    const float& GetSteeringPosition() const;
    const float& GetMaxSteeringPosition() const;
    const float& GetMaxSteeringAngularVelocity() const;
    const float& GetTurningRadius() const;
    
private:
    virtual void UpdateVelocityX();
    virtual void UpdateVelocityY();
    virtual void UpdateVelocityZ();
    virtual void UpdateRotationalVelocityXY();
    virtual void UpdateRotationalVelocityZ();

    static const float DEFAULT_MAX_STEERING_POSITION;
    static const float DEFAULT_MAX_STEERING_ANGULAR_VELOCITY;
    static const float DEFAULT_TURNING_RADIUS;

    float m_acceleratorPosition;
    float m_desiredSteeringPosition;
    float m_steeringPosition;
    float m_maxSteeringPosition;
    float m_maxSteeringAngularVelocity;
    float m_turningRadius;
};