#pragma once
#include "GraphicsCommands/GraphicsCommandBase.h"

class Mesh;
class Material;
struct PipelineStateObject;

class RenderInstancedMesh : public GraphicsCommandBase
{
public:
	RenderInstancedMesh(const std::shared_ptr<Mesh> aMesh,
						const std::vector<std::shared_ptr<Material>>& aMaterialList,
						const Math::Matrix4x4f& aTransform,
						DynamicVertexBuffer* aInstanceBuffer,
						unsigned aMeshCount);
	void Execute() override;
private:
	std::shared_ptr<Mesh> myMesh;
	std::vector<std::shared_ptr<Material>> myMaterialList;
	Math::Matrix4x4f myTransform;
	DynamicVertexBuffer* myInstanceBuffer = nullptr;
	unsigned myMeshCount = 0;
};

