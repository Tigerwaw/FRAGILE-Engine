#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix.hpp"

class Sprite;
class Texture;
class Material;

class RenderSprite : GraphicsCommandBase
{
public:
	RenderSprite(const std::shared_ptr<Material> aMaterial, const Math::Matrix4x4f& aTransform);
	RenderSprite(const std::shared_ptr<Texture> aTexture, const Math::Matrix4x4f& aTransform);
	void Execute() override;
private:
	std::shared_ptr<Material> myMaterial;
	std::shared_ptr<Texture> myTexture;
	Math::Matrix4x4f myTransform;
};

