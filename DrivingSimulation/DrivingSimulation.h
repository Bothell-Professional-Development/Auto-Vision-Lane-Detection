#pragma once

#include <mutex>

#include "Composition.h"
#include "DebugDisplay.h"
#include "DrivingMovementControls.h"
#include "Light.h"
#include "Player.h"
#include "SkyDome.h"
#include "Sprite.h"
#include "Terrain.h"
#include "Text.h"
#include "Track.h"

class DrivingSimulation : public Composition
{
public:
    DrivingSimulation();
    DrivingSimulation(const DrivingSimulation& other);
    ~DrivingSimulation();

    virtual bool Initialize(const bool fullScreen,
                            const bool vSync,
                            const unsigned int screenWidth,
                            const unsigned int screenHeight,
                            const float screenDepth,
                            const float screenNear,
                            const D3D11_FILTER samplerFilteringType,
                            const unsigned int maxAnisotropy);

    virtual void Shutdown();
    virtual bool Frame();

    bool SetTrackCaptureResolution(const unsigned int width,
                                   const unsigned int height);

    const bool& IsRenderingTrackToTexture() const;

    DrivingMovementControls* GetDrivingMovementControls() const;
    const RenderTexture* GetTrackCapture() const;

    void GetTrackRasterCopy(unsigned char* copyDestination);

private:
    virtual bool Render();
    virtual bool RenderScene();
    bool RenderTrackToTexture();

    bool m_renderTrackToTexture;

    DrivingMovementControls* m_controls;
    Player* m_player;

    RenderTexture* m_trackCapture;
    std::mutex m_trackCaptureMutex;

    Sprite* m_steeringWheel;

    //refactor into Scene class?
    Light* m_light;
    SkyDome* m_skyDome;
    Terrain* m_terrain;
    Track* m_track;
    //std::vector<Model> m_entities;

    DebugDisplay* m_debugDisplay;
    //Text* m_text;
};