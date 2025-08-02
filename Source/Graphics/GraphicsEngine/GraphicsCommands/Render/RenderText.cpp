#include "GraphicsEngine.pch.h"
#include "RenderText.h"
#include "Objects/Text/Text.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderText::RenderText(const TextData& aTextData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderText Copy Constructor");
	myData = aTextData;
}

RenderText::RenderText(TextData&& aTextData)
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderText Move Constructor");
	myData = std::move(aTextData);
}

void RenderText::Execute()
{
	PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderText Execute");
	assert(myData.text->GetTexture() && "Text object has no texture!");

	ObjectBuffer ob;
	ob.World = myData.transform;

	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, ob);
	GraphicsEngine::Get().GetDrawer().RenderText(*myData.text);
}

void RenderText::Destroy()
{
	myData.text = nullptr;
}
