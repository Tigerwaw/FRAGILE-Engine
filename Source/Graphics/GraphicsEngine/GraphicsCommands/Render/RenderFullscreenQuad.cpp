#include "GraphicsEngine.pch.h"
#include "RenderFullscreenQuad.h"

RenderFullscreenQuad::RenderFullscreenQuad()
{
}

void RenderFullscreenQuad::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderFullscreenQuad Execute");
	GraphicsEngine::Get().GetDrawer().RenderQuad();
}