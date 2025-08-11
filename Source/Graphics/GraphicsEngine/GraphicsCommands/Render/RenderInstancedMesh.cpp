#include "GraphicsEngine.pch.h"
#include "RenderInstancedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderInstancedMesh::RenderInstancedMesh(const std::shared_ptr<Mesh> aMesh,
                                         const std::vector<std::shared_ptr<Material>>& aMaterialList,
                                         const Math::Matrix4x4f& aTransform,
                                         DynamicVertexBuffer* aInstanceBuffer,
                                         unsigned aMeshCount) :
    myMaterialList(aMaterialList)
{
    myMesh = aMesh;
    myTransform = aTransform;
    myInstanceBuffer = aInstanceBuffer;
    myMeshCount = aMeshCount;
}

void RenderInstancedMesh::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMesh Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderInstancedMesh(*myMesh, myMeshCount, myMaterialList, *myInstanceBuffer);
}

void RenderInstancedMesh::Destroy()
{
    myMesh = nullptr;
    myMaterialList.~vector();
}
