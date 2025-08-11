#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class BeginEvent : public GraphicsCommandBase
{
public:
    BeginEvent(std::string_view aEventName);
    void Execute() override;
private:
    std::string_view myEventName;
};

