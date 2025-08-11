#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class GBuffer;
class Texture;

class SetGBufferAsRenderTarget : public GraphicsCommandBase
{
public:
    SetGBufferAsRenderTarget();
    void Execute() override;
private:
    std::shared_ptr<Texture> myAlbedo;
    std::shared_ptr<Texture> myMaterial;
    std::shared_ptr<Texture> myEffects;
    std::shared_ptr<Texture> myWorldNormal;
    std::shared_ptr<Texture> myWorldPosition;
};

