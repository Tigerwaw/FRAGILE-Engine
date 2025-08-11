#include "GraphicsEngine.pch.h"
#include "RenderTrail.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderTrail::RenderTrail(const std::vector<TrailEmitter>& aEmitters, const Math::Matrix4x4f& aTransform) : 
    myEmitters(aEmitters)
{
    myTransform = aTransform;
}

void RenderTrail::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderTrail Execute");
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : myEmitters)
    {
        gfx.GetDrawer().RenderTrailEmitter(emitter);
    }
}