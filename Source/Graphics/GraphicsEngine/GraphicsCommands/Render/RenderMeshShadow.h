#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"
#include "Math/Matrix4x4.hpp"

class Mesh;

class RenderMeshShadow : GraphicsCommandBase
{
public:
	RenderMeshShadow(const std::shared_ptr<Mesh> aMesh, const Math::Matrix4x4f& aTransform);
	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<Mesh> myMesh;
	Math::Matrix4x4f myTransform;
};

