#include "GraphicsEngine.pch.h"
#include "SetRenderTarget.h"

SetRenderTarget::SetRenderTarget(std::shared_ptr<Texture> aRenderTarget, std::shared_ptr<Texture> aDepthStencil, bool aClearRenderTarget, bool aClearDepthStencil)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetRenderTarget Constructor");
    myRenderTarget = aRenderTarget;
    myDepthStencil = aDepthStencil;
    myClearRenderTarget = aClearRenderTarget;
    myClearDepthStencil = aClearDepthStencil;
}

void SetRenderTarget::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetRenderTarget Execute");
    GraphicsEngine::Get().SetRenderTarget(myRenderTarget, myDepthStencil, myClearRenderTarget, myClearDepthStencil);
}