#include "GraphicsEngine.pch.h"
#include "SetTextureResource.h"

#include "Objects/Texture.h"

SetTextureResource::SetTextureResource(unsigned aSlot, std::shared_ptr<Texture> aTexture)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetTextureResource Constructor");
    mySlot = aSlot;
    myTexture = aTexture;
}

void SetTextureResource::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetTextureResource Execute");
    GraphicsEngine::Get().SetTextureResource_PS(mySlot, *myTexture);
}
