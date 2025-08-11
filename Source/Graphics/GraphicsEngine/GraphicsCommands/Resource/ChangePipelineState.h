#pragma once
#include "GraphicsCommandBase.h"

struct PipelineStateObject;

class ChangePipelineState : public GraphicsCommandBase
{
public:
    ChangePipelineState(std::shared_ptr<PipelineStateObject> aPipelineStateObject);
    void Execute() override;
private:
    std::shared_ptr<PipelineStateObject> myPipelineStateObject;
};