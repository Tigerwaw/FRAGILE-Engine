#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class Texture;

class SetTextureResource : public GraphicsCommandBase
{
public:
    SetTextureResource(unsigned aSlot, std::shared_ptr<Texture> aTexture);
    void Execute() override;
private:
    unsigned mySlot;
    std::shared_ptr<Texture> myTexture;
};

