#include "GraphicsEngine.pch.h"
#include "RenderSpritesheet.h"

#include "GraphicsEngine.h"
#include "Objects/ConstantBuffers/SpriteBuffer.h"
#include "Objects/Spritesheet.h"
#include "Objects/Texture.h"
#include "Objects/Material.h"

RenderSpritesheet::RenderSpritesheet(const std::shared_ptr<Material> aMaterial, 
                                     const std::shared_ptr<Texture> aTexture, 
                                     const Math::Matrix4x4f& aTransform,
                                     const Math::Vector2f& aSheetDimensions, 
                                     float aCurrentFrame)
{
    myMaterial = aMaterial;
    myTexture = aTexture;
    myTransform = aTransform;
    mySheetDimensions = aSheetDimensions;
    myCurrentFrame = aCurrentFrame;
}

void RenderSpritesheet::Execute()
{
    PIXScopedEvent(PIX_COLOR_INDEX(1), "GFXCMD RenderSpritesheet Execute");
    if (!myTexture && !myMaterial) return;

    SpriteBuffer spriteBufferData;
    spriteBufferData.Matrix = myTransform;
    spriteBufferData.CurrentFrame = myCurrentFrame;
    spriteBufferData.SpriteSheetDimensions = mySheetDimensions;
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

void RenderSpritesheet::Destroy()
{
    myMaterial = nullptr;
    myTexture = nullptr;
}
