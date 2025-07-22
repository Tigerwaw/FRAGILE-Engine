#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderSkybox : GraphicsCommandBase
{
public:
	struct RenderSkyboxData
	{
		std::shared_ptr<Mesh> mesh;
		Math::Matrix4x4f transform;
		std::shared_ptr<Texture> texture;
	};

	RenderSkybox(const RenderSkyboxData& aSkyboxData);
	RenderSkybox(RenderSkyboxData&& aSkyboxData);
	void Execute() override;
	void Destroy() override;
private:
	RenderSkyboxData myData;
};

