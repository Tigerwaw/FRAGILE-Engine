#include "GraphicsEngine.pch.h"
#include "SetGBufferAsRenderTarget.h"
#include "Objects/GBuffer.h"
#include "Objects/Texture.h"

SetGBufferAsRenderTarget::SetGBufferAsRenderTarget()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetGBufferAsRenderTarget Constructor");
	GBuffer& gBuffer = GraphicsEngine::Get().GetGBuffer();
	myAlbedo = gBuffer.GetAlbedo();
	myMaterial = gBuffer.GetMaterial();
	myEffects = gBuffer.GetEffects();
	myWorldNormal = gBuffer.GetWorldNormal();
	myWorldPosition = gBuffer.GetWorldPosition();
}

void SetGBufferAsRenderTarget::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD SetGBufferAsRenderTarget Execute");
	std::vector<std::shared_ptr<Texture>> RTs;
	RTs.emplace_back(myAlbedo);
	RTs.emplace_back(myMaterial);
	RTs.emplace_back(myEffects);
	RTs.emplace_back(myWorldNormal);
	RTs.emplace_back(myWorldPosition);
	GraphicsEngine::Get().SetRenderTargets(RTs, GraphicsEngine::Get().GetDepthBuffer(), true, true);
}