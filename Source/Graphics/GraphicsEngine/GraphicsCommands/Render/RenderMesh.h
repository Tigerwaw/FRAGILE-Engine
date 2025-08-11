#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderMesh : GraphicsCommandBase
{
public:
	RenderMesh(const std::shared_ptr<Mesh> aMesh,
			   const std::vector<std::shared_ptr<Material>>& aMaterialList,
			   const Math::Matrix4x4f& aTransform,
			   const Math::Vector4f& aCustomShaderParams_1,
			   const Math::Vector4f& aCustomShaderParams_2);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> myMesh;
	std::vector<std::shared_ptr<Material>> myMaterialList;
	Math::Matrix4x4f myTransform;
	Math::Vector4f myCustomShaderParams_1;
	Math::Vector4f myCustomShaderParams_2;
};

