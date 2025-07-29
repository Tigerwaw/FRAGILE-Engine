#include "GraphicsEngine.pch.h"
#include "RenderAnimatedMesh.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/AnimationBuffer.h"

RenderAnimatedMesh::RenderAnimatedMesh(const AnimMeshRenderData& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMesh Copy Constructor");
    myData = aModelData;
}

RenderAnimatedMesh::RenderAnimatedMesh(AnimMeshRenderData&& aModelData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMesh Move Constructor");
    myData = std::move(aModelData);
}

void RenderAnimatedMesh::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderAnimatedMesh Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.hasSkinning = true;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    AnimationBuffer animBufferData;
    memcpy_s(animBufferData.JointTransforms, sizeof(Math::Matrix4x4<float>) * 128, myData.jointTransforms.data(), sizeof(Math::Matrix4x4<float>) * 128);
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::AnimationBuffer, animBufferData);

    GraphicsEngine::Get().GetDrawer().RenderMesh(*myData.mesh, myData.materialList);
}

void RenderAnimatedMesh::Destroy()
{
    myData.mesh = nullptr;
    myData.materialList.~vector();
}