#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"

RenderAnimatedMesh::RenderAnimatedMesh(const std::shared_ptr<Mesh> aMesh,
                                       const std::vector<std::shared_ptr<Material>>& aMaterialList,
                                       const Math::Matrix4x4f& aTransform,
                                       const std::array<Math::Matrix4x4f, 128>& aJointTransforms) :
    myMaterialList(aMaterialList),
    myJointTransforms(aJointTransforms)
{
    myMesh = aMesh;
    myTransform = aTransform;
}

void RenderAnimatedMesh::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMesh Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(Math::Matrix4x4<float>) * 128, myJointTransforms.data(), sizeof(Math::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMesh(*myMesh, myMaterialList);
}