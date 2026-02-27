#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "Mesh.h"
#include "Vertices/Vertex.h"

using namespace Microsoft::WRL;

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::AddSubmesh(Submesh&& aSubmesh)
{
	mySubmeshes.emplace_back(std::move(aSubmesh));
}

void Mesh::AddLOD(unsigned aSubmeshIndex, std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList)
{
	assert(aSubmeshIndex >= 0 && mySubmeshes.size() > aSubmeshIndex);

	mySubmeshes[aSubmeshIndex].AddLOD(static_cast<unsigned>(mySubmeshes[aSubmeshIndex].myLODs.size()), std::move(aVertexList), std::move(aIndexList));
}

void Mesh::SetSkeleton(Skeleton&& aSkeleton)
{
	mySkeleton = std::move(aSkeleton);
}

void Mesh::InitBoundingBox(Math::Vector3f aMinPoint, Math::Vector3f aMaxPoint)
{
	myBoundingBox.InitWithMinAndMax(aMinPoint, aMaxPoint);
}

void Mesh::Submesh::AddLOD(unsigned aLODLevel, std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList)
{
	const auto [it, status] = myLODs.insert({ aLODLevel, LOD() });
	Submesh::LOD& lod = it->second;
	lod.ScreenPercentage = 1 / pow(4.0f, static_cast<float>(it->first));
	lod.myVertices = std::move(aVertexList);
	lod.myIndices = std::move(aIndexList);
	GraphicsEngine::Get().GetResourceVendor().CreateVertexBuffer("Vertex Buffer", lod.myVertices, lod.myVertexBuffer);
	GraphicsEngine::Get().GetResourceVendor().CreateIndexBuffer("Index Buffer", lod.myIndices, lod.myIndexBuffer);
}

const Mesh::Submesh::LOD& Mesh::Submesh::GetLOD(unsigned aLODLevel) const
{
	if (myLODs.contains(aLODLevel))
		return myLODs.at(aLODLevel);

	return myLODs.begin()->second;
}

unsigned Mesh::Submesh::GetAppropriateLODLevel(float aScreenPercentage) const
{
	unsigned index = 0;
	for (auto it = myLODs.begin(); it != myLODs.end(); ++it)
	{
		if (aScreenPercentage >= it->second.ScreenPercentage)
			break;

		if (index == myLODs.size() - 1)
			break;

		index++;
	}

	return index;
}