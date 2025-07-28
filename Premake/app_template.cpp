#include "Enginepch.h"
#include "APPNAME.h"
#include <Application/AppSettings.h>
#include <GameEngine/Engine.h>

Application* CreateApplication()
{
	AppSettings::LoadSettings(std::filesystem::current_path() / APP_SETTINGS_PATH);
    return new APPNAME();
}

void APPNAME::InitializeApplication()
{

}

void APPNAME::UpdateApplication()
{

}

void APPNAME::UpdateDebug()
{

}