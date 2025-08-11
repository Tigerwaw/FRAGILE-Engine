#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderSkybox : GraphicsCommandBase
{
public:
	RenderSkybox(const std::shared_ptr<Mesh> aMesh, const std::shared_ptr<Texture> aTexture, const Math::Matrix4x4f& aTransform);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> myMesh;
	std::shared_ptr<Texture> myTexture;
	Math::Matrix4x4f myTransform;
};

