#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class ClearTextureResource : public GraphicsCommandBase
{
public:
    ClearTextureResource(unsigned aSlot);
    void Execute() override;
private:
    unsigned mySlot;
};

