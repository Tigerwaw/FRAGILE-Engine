#include "GraphicsEngine.pch.h"
#include "SetGBufferAsResource.h"
#include "Objects/GBuffer.h"
#include "Objects/Texture.h"

SetGBufferAsResource::SetGBufferAsResource()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetGbufferAsResource Constructor");
	GBuffer& gBuffer = GraphicsEngine::Get().GetGBuffer();
	myAlbedo = gBuffer.GetAlbedo();
	myMaterial = gBuffer.GetMaterial();
	myEffects = gBuffer.GetEffects();
	myWorldNormal = gBuffer.GetWorldNormal();
	myWorldPosition = gBuffer.GetWorldPosition();
}

void SetGBufferAsResource::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetGBufferAsResource Execute");
	GraphicsEngine::Get().SetTextureResource_PS(0, *myAlbedo);
	GraphicsEngine::Get().SetTextureResource_PS(1, *myMaterial);
	GraphicsEngine::Get().SetTextureResource_PS(2, *myEffects);
	GraphicsEngine::Get().SetTextureResource_PS(3, *myWorldNormal);
	GraphicsEngine::Get().SetTextureResource_PS(4, *myWorldPosition);
}