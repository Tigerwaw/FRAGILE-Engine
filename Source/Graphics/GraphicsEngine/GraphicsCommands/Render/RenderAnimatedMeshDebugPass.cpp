#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMeshDebugPass.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"

RenderAnimatedMeshDebugPass::RenderAnimatedMeshDebugPass(const AnimMeshRenderData& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMeshDebugPass Copy Constructor");
    myData = aModelData;
}

RenderAnimatedMeshDebugPass::RenderAnimatedMeshDebugPass(AnimMeshRenderData&& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMeshDebugPass Move Constructor");
    myData = std::move(aModelData);
}

void RenderAnimatedMeshDebugPass::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMeshDebugPass Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(Math::Matrix4x4<float>) * 128, myData.jointTransforms.data(), sizeof(Math::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMeshDebugPass(*myData.mesh, myData.materialList);
}

void RenderAnimatedMeshDebugPass::Destroy()
{
    myData.mesh = nullptr;
    myData.materialList.~vector();
}