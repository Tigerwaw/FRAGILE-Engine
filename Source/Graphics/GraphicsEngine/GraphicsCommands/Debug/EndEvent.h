#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class EndEvent : public GraphicsCommandBase
{
public:
    EndEvent();
    void Execute() override;
};

