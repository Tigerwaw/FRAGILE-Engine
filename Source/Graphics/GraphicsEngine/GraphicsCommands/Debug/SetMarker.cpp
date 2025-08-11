#include "GraphicsEngine.pch.h"
#include "SetMarker.h"

SetMarker::SetMarker(std::string_view aMarkerName)
{
	myMarkerName = aMarkerName;
}

void SetMarker::Execute()
{
	GraphicsEngine::Get().SetMarker(myMarkerName);
}