#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMeshShadow.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"

RenderAnimatedMeshShadow::RenderAnimatedMeshShadow(const AnimMeshShadowRenderData& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMeshShadow Copy Constructor");
    myData = aModelData;
}

RenderAnimatedMeshShadow::RenderAnimatedMeshShadow(AnimMeshShadowRenderData&& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMeshShadow Move Constructor");
    myData = std::move(aModelData);
}

void RenderAnimatedMeshShadow::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMeshShadow Execute");
    
    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(Math::Matrix4x4<float>) * 128, myData.jointTransforms.data(), sizeof(Math::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMeshShadow(*myData.mesh);
}

void RenderAnimatedMeshShadow::Destroy()
{
    myData.mesh = nullptr;
}