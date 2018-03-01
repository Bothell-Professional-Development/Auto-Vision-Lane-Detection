#pragma once

#include "Camera.h"
#include "MovementControls.h"
#include "Position.h"

class Player
{
public:
    Player();
    Player(const Player& other);
    ~Player();

    bool Initialize(MovementControls* controls);
    void Shutdown();

    Camera* GetCamera() const;
    MovementControls* GetMovementControls() const;
    Position* GetPosition() const;

    void Frame(const float frameTime);

private:

    Camera* m_camera;
    MovementControls* m_movementControls;
    Position* m_position;
};
