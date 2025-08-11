#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Objects/ConstantBuffers/PostProcessBuffer.h"

class UpdatePostProcessBuffer : public GraphicsCommandBase
{
public:
    UpdatePostProcessBuffer();
    void Execute() override;
};

