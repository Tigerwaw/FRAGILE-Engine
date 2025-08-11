#include "GraphicsEngine.pch.h"
#include "RenderSprite.h"

#include "GraphicsEngine.h"
#include "Objects/ConstantBuffers/SpriteBuffer.h"
#include "Objects/Sprite.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"

RenderSprite::RenderSprite(const std::shared_ptr<Material> aMaterial, const Math::Matrix4x4f& aTransform)
{
    myMaterial = aMaterial;
    myTransform = aTransform;
}

RenderSprite::RenderSprite(const std::shared_ptr<Texture> aTexture, const Math::Matrix4x4f& aTransform)
{
    myTexture = aTexture;
    myTransform = aTransform;
}

void RenderSprite::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSprite Execute");
    if (!myTexture && !myMaterial) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Matrix = myTransform;
    GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::SpriteBuffer, spriteBufferData);

    if (myMaterial)
    {
        GraphicsEngine::Get().ChangePipelineState(myMaterial->GetPSO());
        GraphicsEngine::Get().SetTextureResource_PS(0, myMaterial->GetTexture(Material::TextureType::Albedo));
        GraphicsEngine::Get().GetDrawer().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
    else
    {
        GraphicsEngine::Get().SetTextureResource_PS(0, *myTexture);
        GraphicsEngine::Get().GetDrawer().RenderSprite();
        GraphicsEngine::Get().ClearTextureResource_PS(0);
    }
}