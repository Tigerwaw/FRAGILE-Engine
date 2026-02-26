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
	lod.Distance = aLODLevel * 1000.0f * 1000.0f; // TEMP Distance heuristic
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

unsigned Mesh::Submesh::GetAppropriateLODLevel(float aDistance) const
{
	unsigned index = static_cast<unsigned>(myLODs.size() - 1);
	for (auto it = myLODs.rbegin(); it != myLODs.rend(); ++it)
	{
		if (aDistance >= it->second.Distance)
			return index;

		index--;
	}

	return index;
}