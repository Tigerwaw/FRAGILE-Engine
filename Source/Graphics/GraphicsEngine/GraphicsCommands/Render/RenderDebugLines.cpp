#include "GraphicsEngine.pch.h"
#include "RenderDebugLines.h"
#include "Objects/DynamicVertexBuffer.h"
#include "Objects/Vertices/DebugLineVertex.h"

RenderDebugLines::RenderDebugLines(const std::vector<DebugLineVertex>& aLineVertices, std::shared_ptr<DynamicVertexBuffer> aLineBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderDebugLines Copy Constructor");
	myLineVertices = aLineVertices;
	myLineBuffer = aLineBuffer;
}

RenderDebugLines::RenderDebugLines(std::vector<DebugLineVertex>&& aLineVertices, std::shared_ptr<DynamicVertexBuffer> aLineBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderDebugLines Move Constructor");
	myLineVertices = std::move(aLineVertices);
	myLineBuffer = aLineBuffer;
}

void RenderDebugLines::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderDebugLines Execute");
	GraphicsEngine::Get().UpdateDynamicVertexBuffer(myLineVertices, *myLineBuffer);
	GraphicsEngine::Get().GetDrawer().RenderDebugLines(*myLineBuffer, static_cast<unsigned>(myLineVertices.size()));
}