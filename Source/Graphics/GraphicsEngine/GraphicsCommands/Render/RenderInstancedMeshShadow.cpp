#include "GraphicsEngine.pch.h"
#include "RenderInstancedMeshShadow.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderInstancedMeshShadow::RenderInstancedMeshShadow(const InstancedMeshShadowRenderData& aInstancedModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshShadow Copy Constructor");
    myData = aInstancedModelData;
}

RenderInstancedMeshShadow::RenderInstancedMeshShadow(InstancedMeshShadowRenderData&& aInstancedModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshShadow Move Constructor");
    myData = std::move(aInstancedModelData);
}

void RenderInstancedMeshShadow::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshShadow Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderInstancedMeshShadow(*myData.mesh, myData.meshCount, *myData.instanceBuffer);
}

void RenderInstancedMeshShadow::Destroy()
{
    myData.mesh = nullptr;
}
