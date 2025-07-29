#include "Enginepch.h"
#include "InstancedModel.h"
#include "Model.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "GraphicsEngine.h"
#include "Objects/Mesh.h"

#undef min
#undef max

InstancedModel::InstancedModel(unsigned aMaxInstances)
{
	myMaxInstances = aMaxInstances;
	SetMaterialOnSlot(0, GraphicsEngine::Get().GetDefaultMaterial());
}

void InstancedModel::Start()
{
	myMeshTransformBuffer.CreateBuffer("Instanced Model Buffer", myMeshTransforms, myMaxInstances);
}

void InstancedModel::Update()
{
}

void InstancedModel::SetMesh(std::shared_ptr<Mesh> aMesh)
{
	myMesh = aMesh;
	for (auto& element : myMesh->GetElements())
	{
		SetMaterialOnSlot(element.MaterialIndex, GraphicsEngine::Get().GetDefaultMaterial());
	}
}

void InstancedModel::AddInstance(const Math::Matrix4x4f& aTransform)
{
	AddInstanceNoBufferUpdate(aTransform);
	UpdateInstanceBuffer();
}

void InstancedModel::AddInstanceNoBufferUpdate(const Math::Matrix4x4f& aTransform)
{
	myMeshTransforms.emplace_back(aTransform);

	auto transform = gameObject->GetComponent<Transform>();
	Math::AABB3D<float> meshAABB = myMesh->GetBoundingBox().GetAABBinNewSpace(aTransform);

	Math::Vector3f aabbMin = myBoundingBox.GetMin();
	Math::Vector3f aabbMax = myBoundingBox.GetMax();
	aabbMin.x = std::min(aabbMin.x, meshAABB.GetMin().x);
	aabbMin.y = std::min(aabbMin.y, meshAABB.GetMin().y);
	aabbMin.z = std::min(aabbMin.z, meshAABB.GetMin().z);
	aabbMax.x = std::max(aabbMax.x, meshAABB.GetMax().x);
	aabbMax.y = std::max(aabbMax.y, meshAABB.GetMax().y);
	aabbMax.z = std::max(aabbMax.z, meshAABB.GetMax().z);

	myBoundingBox.InitWithMinAndMax(aabbMin, aabbMax);
}

void InstancedModel::UpdateInstanceBuffer()
{
	myMeshTransformBuffer.UpdateVertexBuffer(myMeshTransforms);
}

void InstancedModel::SetMaterialOnSlot(unsigned aSlot, std::shared_ptr<Material> aMaterial)
{
	myMaterials.emplace_back(aMaterial);

	if (mySlotToIndex.find(aSlot) != mySlotToIndex.end())
	{
		myMaterials.erase(myMaterials.begin() + mySlotToIndex.at(aSlot));
		mySlotToIndex.erase(aSlot);
	}

	mySlotToIndex.emplace(aSlot, static_cast<unsigned>(myMaterials.size() - 1));
}

bool InstancedModel::Serialize(nl::json&)
{
	return false;
}

bool InstancedModel::Deserialize(nl::json&)
{
	return false;
}
