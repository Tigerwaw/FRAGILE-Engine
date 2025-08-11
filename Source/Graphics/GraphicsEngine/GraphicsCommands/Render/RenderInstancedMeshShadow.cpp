#include "GraphicsEngine.pch.h"
#include "RenderInstancedMeshShadow.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"

RenderInstancedMeshShadow::RenderInstancedMeshShadow(const std::shared_ptr<Mesh> aMesh, 
                                                     const Math::Matrix4x4f& aTransform, 
                                                     DynamicVertexBuffer* aInstanceBuffer, 
                                                     unsigned aMeshCount)
{
    myMesh = aMesh;
    myTransform = aTransform;
    myInstanceBuffer = aInstanceBuffer;
    myMeshCount = aMeshCount;
}

void RenderInstancedMeshShadow::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshShadow Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderInstancedMeshShadow(*myMesh, myMeshCount, *myInstanceBuffer);
}