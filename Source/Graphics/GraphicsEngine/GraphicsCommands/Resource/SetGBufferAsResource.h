#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class GBuffer;
class Texture;

class SetGBufferAsResource : public GraphicsCommandBase
{
public:
    SetGBufferAsResource();
    void Execute() override;
private:
    std::shared_ptr<Texture> myAlbedo;
    std::shared_ptr<Texture> myMaterial;
    std::shared_ptr<Texture> myEffects;
    std::shared_ptr<Texture> myWorldNormal;
    std::shared_ptr<Texture> myWorldPosition;
};

