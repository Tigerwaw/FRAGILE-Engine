#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;

class RenderAnimatedMeshShadow : GraphicsCommandBase
{
public:
	RenderAnimatedMeshShadow(const std::shared_ptr<Mesh> aMesh, 
						     const Math::Matrix4x4f& aTransform, 
						     const std::array<Math::Matrix4x4f, 128>& aJointTransforms);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> myMesh;
	Math::Matrix4x4f myTransform;
	std::array<Math::Matrix4x4f, 128> myJointTransforms;
};

