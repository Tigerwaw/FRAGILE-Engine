#include "GraphicsEngine.pch.h"
#include "RenderTrail.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderTrail::RenderTrail(const TrailData& aTrailData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderTrail Copy Constructor");
    myData = aTrailData;
}

RenderTrail::RenderTrail(TrailData&& aTrailData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderTrail Move Constructor");
    myData = std::move(aTrailData);
}

void RenderTrail::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderTrail Execute");
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : myData.emitters)
    {
        gfx.GetDrawer().RenderTrailEmitter(emitter);
    }
}

void RenderTrail::Destroy()
{
    myData.emitters.~vector();
}
