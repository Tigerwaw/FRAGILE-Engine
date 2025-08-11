#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class RenderFullscreenQuad : GraphicsCommandBase
{
public:
	RenderFullscreenQuad();
	void Execute() override;
};