#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class Mesh;

class RenderInstancedMeshShadow : public GraphicsCommandBase
{
public:
	RenderInstancedMeshShadow(const std::shared_ptr<Mesh> aMesh, 
							  const Math::Matrix4x4f& aTransform, 
							  DynamicVertexBuffer* aInstanceBuffer, 
							  unsigned aMeshCount);
	void Execute() override;
private:
	std::shared_ptr<Mesh> myMesh;
	Math::Matrix4x4f myTransform;
	DynamicVertexBuffer* myInstanceBuffer = nullptr;
	unsigned myMeshCount = 0;
};

