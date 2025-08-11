#include "GraphicsEngine.pch.h"
#include "RenderParticles.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderParticles::RenderParticles(const std::vector<ParticleEmitter>& aEmitters, const Math::Matrix4x4f& aTransform) 
    : myEmitters(aEmitters)
{
    myTransform = aTransform;
}

void RenderParticles::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderParticles Execute");
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : myEmitters)
    {
        gfx.GetDrawer().RenderParticleEmitter(emitter);
    }
}

void RenderParticles::Destroy()
{
    myEmitters.~vector();
}
