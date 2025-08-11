#include "GraphicsEngine.pch.h"
#include "BeginEvent.h"

BeginEvent::BeginEvent(std::string_view aEventName)
{
	myEventName = aEventName;
}

void BeginEvent::Execute()
{
	GraphicsEngine::Get().BeginEvent(myEventName);
}