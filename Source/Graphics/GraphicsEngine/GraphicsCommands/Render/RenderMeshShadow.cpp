#include "GraphicsEngine.pch.h"
#include "RenderMeshShadow.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderMeshShadow::RenderMeshShadow(const std::shared_ptr<Mesh> aMesh, const Math::Matrix4x4f& aTransform)
{
    myMesh = aMesh;
    myTransform = aTransform;
}

void RenderMeshShadow::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMeshShadow Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMeshShadow(*myMesh);
}

void RenderMeshShadow::Destroy()
{
    myMesh = nullptr;
}