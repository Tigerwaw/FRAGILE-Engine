#include "GraphicsEngine.pch.h"
#include "RenderParticles.h"

#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderParticles::RenderParticles(const RenderParticlesData& aParticleSystemData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderParticles Copy Constructor");
    myData = aParticleSystemData;
}

RenderParticles::RenderParticles(RenderParticlesData&& aParticleSystemData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderParticles Move Constructor");
    myData = std::move(aParticleSystemData);
}

void RenderParticles::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderParticles Execute");
    GraphicsEngine& gfx = GraphicsEngine::Get();

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    gfx.UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    for (auto& emitter : myData.emitters)
    {
        gfx.GetDrawer().RenderParticleEmitter(emitter);
    }
}

void RenderParticles::Destroy()
{
    myData.emitters.~vector();
}
