#include "stdafx.h"

#include "Player.h"
#include "System.h"

Player::Player() :
    m_camera(NULL),
    m_movementControls(NULL),
    m_position(NULL){}
Player::Player(const Player& other){}
Player::~Player(){}

bool Player::Initialize(MovementControls* controls)
{
    bool success = true;

    m_camera = new Camera();

    m_position = new Position();

    if(controls)
    {
        m_movementControls = controls;
    }
    else
    {
        System::GetInstance().ShowMessage(L"Player::Initialize: Movement controls instance not valid",
                                          L"Error");
        success = false;
    }

    return success;
}

void Player::Shutdown()
{
    if(m_camera)
    {
        delete m_camera;
        m_camera = NULL;
    }

    if(m_position)
    {
        delete m_position;
        m_position = NULL;
    }
}

Camera* Player::GetCamera() const
{
    return m_camera;
}

MovementControls* Player::GetMovementControls() const
{
    return m_movementControls;
}

Position* Player::GetPosition() const
{
    return m_position;
}

void Player::Frame(const float frameTime)
{
    m_movementControls->Frame(frameTime);

    m_position->Frame(frameTime,
                      m_movementControls->GetVelocity(),
                      m_movementControls->GetRotationalVelocity());

    DirectX::XMFLOAT3 currentPosition = m_position->GetPosition();
    DirectX::XMFLOAT3 currentRotation = m_position->GetRotation();
    
    m_camera->SetPosition(currentPosition.x,
                          currentPosition.y,
                          currentPosition.z);

    m_camera->SetRotation(currentRotation.x,
                          currentRotation.y,
                          currentRotation.z);
}