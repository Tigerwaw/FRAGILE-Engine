#include "GraphicsEngine.pch.h"
#include "SetDefaultRenderTarget.h"

SetDefaultRenderTarget::SetDefaultRenderTarget()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetDefaultRenderTarget Constructor");
    myRenderTarget = GraphicsEngine::Get().GetBackBuffer();
    myDepthStencil = GraphicsEngine::Get().GetDepthBuffer();
    myClearRenderTarget = true;
    myClearDepthStencil = true;
}

void SetDefaultRenderTarget::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetDefaultRenderTarget Execute");
    GraphicsEngine::Get().SetRenderTarget(myRenderTarget, myDepthStencil, myClearRenderTarget, myClearDepthStencil);
}