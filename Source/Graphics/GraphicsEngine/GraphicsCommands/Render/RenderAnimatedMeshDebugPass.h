#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderAnimatedMeshDebugPass : GraphicsCommandBase
{
public:
	RenderAnimatedMeshDebugPass(const std::shared_ptr<Mesh> aMesh, 
								const std::vector<std::shared_ptr<Material>>& aMaterialList, 
								const Math::Matrix4x4f& aTransform, 
								const std::array<Math::Matrix4x4f, 128>& aJointTransforms,
								float aLODHeuristic);
	void Execute() override;
private:
	std::shared_ptr<Mesh> myMesh;
	std::vector<std::shared_ptr<Material>> myMaterialList;
	Math::Matrix4x4f myTransform;
	std::array<Math::Matrix4x4f, 128> myJointTransforms;
	float myLODHeuristic;
};

