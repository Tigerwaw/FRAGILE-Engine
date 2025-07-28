#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class GrassRendering : public Application
{
public:
    void InitializeApplication() override;
    void UpdateApplication() override;
    void UpdateDebug() override;

private:
    unsigned currentDebugMode = 0;
};