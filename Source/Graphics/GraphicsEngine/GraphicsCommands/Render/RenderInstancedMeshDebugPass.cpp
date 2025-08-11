#include "GraphicsEngine.pch.h"
#include "RenderInstancedMeshDebugPass.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderInstancedMeshDebugPass::RenderInstancedMeshDebugPass(const std::shared_ptr<Mesh> aMesh,
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

void RenderInstancedMeshDebugPass::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderInstancedMeshDebugPass Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = false;
    objBufferData.isInstanced = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderInstancedMeshDebugPass(*myMesh, myMeshCount, myMaterialList, *myInstanceBuffer);
}