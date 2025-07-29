#include "GraphicsEngine.pch.h"
#include "RenderMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderMesh::RenderMesh(const RenderMeshData& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMesh Copy Constructor");
    myData = aModelData;
}

RenderMesh::RenderMesh(RenderMeshData&& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMesh Move Constructor");
    myData = std::move(aModelData);
}

void RenderMesh::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMesh Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = false;
    objBufferData.customData_1 = myData.customShaderParams_1;
    objBufferData.customData_2 = myData.customShaderParams_2;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMesh(*myData.mesh, myData.materialList);
}

void RenderMesh::Destroy()
{
    myData.mesh = nullptr;
    myData.materialList.~vector();
}