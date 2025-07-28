#pragma once
#include "GameEngine/Application/Application.h"
#include "GameEngine/Application/EntryPoint.h"

class APPNAME : public Application
{
public:
    void InitializeApplication() override;
    void UpdateApplication() override;
    void UpdateDebug() override;
};