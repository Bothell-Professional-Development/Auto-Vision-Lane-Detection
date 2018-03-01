#pragma once

#include <d3d11.h>

class Renderable
{
public:
    virtual void Shutdown() = 0;
    virtual void Render(ID3D11DeviceContext* context) = 0;

private:
    virtual bool InitializeBuffers(ID3D11Device* device) = 0;
};