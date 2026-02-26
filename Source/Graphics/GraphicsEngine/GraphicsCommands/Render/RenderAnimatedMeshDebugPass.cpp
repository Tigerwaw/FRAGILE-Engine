#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMeshDebugPass.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"

RenderAnimatedMeshDebugPass::RenderAnimatedMeshDebugPass(const std::shared_ptr<Mesh> aMesh, 
                                                         const std::vector<std::shared_ptr<Material>>& aMaterialList, 
                                                         const Math::Matrix4x4f& aTransform, 
                                                         const std::array<Math::Matrix4x4f, 128>& aJointTransforms,
                                                         float aLODHeuristic) :
    myMaterialList(aMaterialList),
    myJointTransforms(aJointTransforms)
{
    myMesh = aMesh;
    myLODHeuristic = aLODHeuristic;
    myTransform = aTransform;
}

void RenderAnimatedMeshDebugPass::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMeshDebugPass Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(Math::Matrix4x4<float>) * 128, myJointTransforms.data(), sizeof(Math::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMeshDebugPass(*myMesh, myMaterialList, myLODHeuristic);
}