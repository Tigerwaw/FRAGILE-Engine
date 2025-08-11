#include "GraphicsEngine.pch.h"
#include "RenderSkybox.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderSkybox::RenderSkybox(const std::shared_ptr<Mesh> aMesh, const std::shared_ptr<Texture> aTexture, const Math::Matrix4x4f& aTransform)
{
    myMesh = aMesh;
    myTexture = aTexture;
    myTransform = aTransform;
}

void RenderSkybox::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSkybox Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myTransform;
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderSkybox(*myMesh, myTexture);
}

void RenderSkybox::Destroy()
{
    myMesh = nullptr;
    myTexture = nullptr;
}