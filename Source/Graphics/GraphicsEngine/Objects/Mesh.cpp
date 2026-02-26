#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "Mesh.h"
#include "Vertices/Vertex.h"

using namespace Microsoft::WRL;

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

void Mesh::AddSubmesh(std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList, unsigned aMaterialIndex)
{
	Submesh& newSubmesh = mySubmeshes.emplace_back();
	newSubmesh.myVertices = std::move(aVertexList);
	newSubmesh.myIndices = std::move(aIndexList);
	newSubmesh.MaterialIndex = aMaterialIndex;
	newSubmesh.NumVertices = static_cast<unsigned>(newSubmesh.myVertices.size());
	newSubmesh.NumIndices = static_cast<unsigned>(newSubmesh.myIndices.size());
	GraphicsEngine::Get().GetResourceVendor().CreateVertexBuffer("Vertex Buffer", newSubmesh.myVertices, newSubmesh.myVertexBuffer);
	GraphicsEngine::Get().GetResourceVendor().CreateIndexBuffer("Index Buffer", newSubmesh.myIndices, newSubmesh.myIndexBuffer);
}

void Mesh::SetSkeleton(Skeleton&& aSkeleton)
{
	mySkeleton = std::move(aSkeleton);
}

void Mesh::InitBoundingBox(Math::Vector3f aMinPoint, Math::Vector3f aMaxPoint)
{
	myBoundingBox.InitWithMinAndMax(aMinPoint, aMaxPoint);
}