#include "GraphicsEngine.pch.h"
#include "UpdateFrameBuffer.h"

UpdateFrameBuffer::UpdateFrameBuffer(const FrameBuffer& aFrameBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateFrameBuffer Copy Constructor");
	myFrameBuffer = aFrameBuffer;
}

UpdateFrameBuffer::UpdateFrameBuffer(FrameBuffer&& aFrameBuffer)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateFrameBuffer Move Constructor");
	myFrameBuffer = std::move(aFrameBuffer);
}

void UpdateFrameBuffer::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD UpdateFrameBuffer Execute");
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, myFrameBuffer);
}