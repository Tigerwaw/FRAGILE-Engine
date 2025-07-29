#include "GraphicsEngine.pch.h"
#include "RenderInstancedMeshDebugPass.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderInstancedMeshDebugPass::RenderInstancedMeshDebugPass(const InstancedMeshRenderData& aInstancedModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshDebugPass Copy Constructor");
    myData = aInstancedModelData;
}

RenderInstancedMeshDebugPass::RenderInstancedMeshDebugPass(InstancedMeshRenderData&& aInstancedModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshDebugPass Move Constructor");
    myData = std::move(aInstancedModelData);
}

void RenderInstancedMeshDebugPass::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshDebugPass Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderInstancedMeshDebugPass(*myData.mesh, myData.meshCount, myData.materialList, *myData.instanceBuffer);
}

void RenderInstancedMeshDebugPass::Destroy()
{
    myData.mesh = nullptr;
    myData.materialList.~vector();
}
