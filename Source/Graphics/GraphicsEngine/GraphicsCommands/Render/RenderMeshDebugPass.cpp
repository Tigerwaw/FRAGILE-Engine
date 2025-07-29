#include "GraphicsEngine.pch.h"
#include "RenderMeshDebugPass.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderMeshDebugPass::RenderMeshDebugPass(const RenderMeshData& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMesh Copy Constructor");
    myData = aModelData;
}

RenderMeshDebugPass::RenderMeshDebugPass(RenderMeshData&& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMeshDebugPass Move Constructor");
    myData = std::move(aModelData);
}

void RenderMeshDebugPass::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMeshDebugPass Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = false;
    objBufferData.customData_1 = myData.customShaderParams_1;
    objBufferData.customData_2 = myData.customShaderParams_2;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMeshDebugPass(*myData.mesh, myData.materialList);
}

void RenderMeshDebugPass::Destroy()
{
    myData.mesh = nullptr;
    myData.materialList.~vector();
}