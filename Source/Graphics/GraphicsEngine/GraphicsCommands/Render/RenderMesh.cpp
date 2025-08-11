#include "GraphicsEngine.pch.h"
#include "RenderMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderMesh::RenderMesh(const std::shared_ptr<Mesh> aMesh,
                       const std::vector<std::shared_ptr<Material>>& aMaterialList,
                       const Math::Matrix4x4f& aTransform,
                       const Math::Vector4f& aCustomShaderParams_1,
                       const Math::Vector4f& aCustomShaderParams_2) :
    myMaterialList(aMaterialList)
{
    myMesh = aMesh;
    myMaterialList = aMaterialList;
    myTransform = aTransform;
    myCustomShaderParams_1 = aCustomShaderParams_1;
    myCustomShaderParams_2 = aCustomShaderParams_2;
}

void RenderMesh::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderMesh Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = false;
    objBufferData.customData_1 = myCustomShaderParams_1;
    objBufferData.customData_2 = myCustomShaderParams_2;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMesh(*myMesh, myMaterialList);
}

void RenderMesh::Destroy()
{
    myMesh = nullptr;
    myMaterialList.~vector();
}