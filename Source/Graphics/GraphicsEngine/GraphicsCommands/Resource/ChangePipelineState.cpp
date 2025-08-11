#include "GraphicsEngine.pch.h"
#include "ChangePipelineState.h"

ChangePipelineState::ChangePipelineState(std::shared_ptr<PipelineStateObject> aPipelineStateObject)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD ChangePipelineState Constructor");
    myPipelineStateObject = aPipelineStateObject;
}

void ChangePipelineState::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD ChangePipelineState Execute");
    GraphicsEngine::Get().ChangePipelineState(myPipelineStateObject);
}