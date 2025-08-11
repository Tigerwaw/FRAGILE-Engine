#include "GraphicsEngine.pch.h"
#include "ClearTextureResource.h"

ClearTextureResource::ClearTextureResource(unsigned aSlot)
{
    mySlot = aSlot;
}

void ClearTextureResource::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD ClearTextureResource Execute");
    GraphicsEngine::Get().ClearTextureResource_PS(mySlot);
}