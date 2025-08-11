#include "GraphicsEngine.pch.h"
#include "EndEvent.h"

EndEvent::EndEvent()
{
}

void EndEvent::Execute()
{
	GraphicsEngine::Get().EndEvent();
}