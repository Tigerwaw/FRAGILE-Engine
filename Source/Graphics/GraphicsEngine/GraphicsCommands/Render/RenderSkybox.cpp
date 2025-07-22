#include "GraphicsEngine.pch.h"
#include "RenderSkybox.h"

#include "GraphicsEngine.h"
#include "Objects/Mesh.h"
#include "Objects/Material.h"
#include "Objects/ConstantBuffers/ObjectBuffer.h"
#include "Objects/ConstantBuffers/MaterialBuffer.h"

RenderSkybox::RenderSkybox(const RenderSkyboxData& aSkyboxData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSkybox Copy Constructor");
    myData = aSkyboxData;
}

RenderSkybox::RenderSkybox(RenderSkyboxData&& aSkyboxData)
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSkybox Move Constructor");
    myData = std::move(aSkyboxData);
}

void RenderSkybox::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSkybox Execute");

    ObjectBuffer objBufferData;
    objBufferData.World = myData.transform;
    objBufferData.WorldInvT = myData.transform.GetFastInverse().GetTranspose();
    objBufferData.hasSkinning = false;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::ObjectBuffer, objBufferData);

    GraphicsEngine::Get().GetDrawer().RenderSkybox(*myData.mesh, myData.texture);
}

void RenderSkybox::Destroy()
{
    myData.mesh = nullptr;
    myData.texture = nullptr;
}