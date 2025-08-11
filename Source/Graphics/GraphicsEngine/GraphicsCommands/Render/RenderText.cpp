#include "GraphicsEngine.pch.h"
#include "RenderText.h"
#include "Objects/Text/Text.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderText::RenderText(const std::shared_ptr<Text> aText, const Math::Matrix4x4f& aTransform)
{
	myText = aText;
	myTransform = aTransform;
}

void RenderText::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderText Execute");
	assert(myText->GetTexture() && "Text object has no texture!");

	ObjectBuffer ob;
	ob.World = myTransform;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, ob);
	GraphicsEngine::Get().GetDrawer().RenderText(*myText);
}

void RenderText::Destroy()
{
	myText = nullptr;
}
