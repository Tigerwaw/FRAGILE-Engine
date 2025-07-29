#include "GraphicsEngine.pch.h"
#include "RenderInstancedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderInstancedMesh::RenderInstancedMesh(const InstancedMeshRenderData& aInstancedModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMesh Copy Constructor");
    myData = aInstancedModelData;
}

RenderInstancedMesh::RenderInstancedMesh(InstancedMeshRenderData&& aInstancedModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMesh Move Constructor");
    myData = std::move(aInstancedModelData);
}

void RenderInstancedMesh::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMesh Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderInstancedMesh(*myData.mesh, myData.meshCount, myData.materialList, *myData.instanceBuffer);
}

void RenderInstancedMesh::Destroy()
{
    myData.mesh = nullptr;
    myData.materialList.~vector();
}
