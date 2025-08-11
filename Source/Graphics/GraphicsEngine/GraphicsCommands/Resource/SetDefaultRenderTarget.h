#pragma once
#include "GraphicsCommandBase.h"

class Texture;

class SetDefaultRenderTarget : public GraphicsCommandBase
{
public:
    SetDefaultRenderTarget();
    void Execute() override;
private:
    std::shared_ptr<Texture> myRenderTarget;
    std::shared_ptr<Texture> myDepthStencil;
    bool myClearRenderTarget;
    bool myClearDepthStencil;
};

