#include "GraphicsEngine.pch.h"
#include "UpdateLightBuffer.h"

UpdateLightBuffer::UpdateLightBuffer(const LightBuffer& aLightBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateLightBuffer Copy Constructor");
	myLightBuffer = aLightBuffer;
}

UpdateLightBuffer::UpdateLightBuffer(LightBuffer&& aLightBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateLightBuffer Move Constructor");
	myLightBuffer = std::move(aLightBuffer);
}

void UpdateLightBuffer::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateLightBuffer Execute");
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::LightBuffer, myLightBuffer);
}