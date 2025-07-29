#include "GraphicsEngine.pch.h"
#include "RenderMeshShadow.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderMeshShadow::RenderMeshShadow(const RenderMeshShadowData& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMeshShadow Copy Constructor");
    myData = aModelData;
}

RenderMeshShadow::RenderMeshShadow(RenderMeshShadowData&& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMeshShadow Move Constructor");
    myData = std::move(aModelData);
}

void RenderMeshShadow::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMeshShadow Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMeshShadow(*myData.mesh);
}

void RenderMeshShadow::Destroy()
{
    myData.mesh = nullptr;
}