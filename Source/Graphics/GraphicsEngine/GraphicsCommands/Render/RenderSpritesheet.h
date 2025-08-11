#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

class Spritesheet;
class Texture;
class Material;

class RenderSpritesheet : GraphicsCommandBase
{
public:
	RenderSpritesheet(const std::shared_ptr<Material> aMaterial, 
		const std::shared_ptr<Texture> aTexture, 
		const Math::Matrix4x4f& aTransform,
		const Math::Vector2f& aSheetDimensions,
		float aCurrentFrame);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Material> myMaterial;
	std::shared_ptr<Texture> myTexture;
	Math::Matrix4x4f myTransform;
	Math::Vector2f mySheetDimensions;
	float myCurrentFrame;
};

