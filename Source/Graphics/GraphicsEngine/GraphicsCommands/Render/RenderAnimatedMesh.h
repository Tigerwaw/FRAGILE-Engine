#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderAnimatedMesh : GraphicsCommandBase
{
public:
	RenderAnimatedMesh(const std::shared_ptr<Mesh> aMesh,
					   const std::vector<std::shared_ptr<Material>>& aMaterialList,
					   const Math::Matrix4x4f& aTransform,
					   const std::array<Math::Matrix4x4f, 128>& aJointTransforms);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> myMesh;
	std::vector<std::shared_ptr<Material>> myMaterialList;
	Math::Matrix4x4f myTransform;
	std::array<Math::Matrix4x4f, 128> myJointTransforms;
};

