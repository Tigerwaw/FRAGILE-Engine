#pragma once
#include "Math/Matrix4x4.hpp"
#include "Math/Vector.hpp"
#include "Math/AABB3D.hpp"

struct ID3D11Buffer;
struct Vertex;

class Mesh
{
public:
	struct Submesh
	{
		friend class Mesh;

		struct LOD
		{
			friend class Mesh;

			float ScreenPercentage;

			unsigned GetNumIndices() const { return static_cast<unsigned>(myIndices.size()); }
			FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return myVertexBuffer; }
			FORCEINLINE const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndexBuffer() const { return myIndexBuffer; }

		private:
			std::vector<Vertex> myVertices;
			std::vector<unsigned> myIndices;

			Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> myIndexBuffer;
		};

		void AddLOD(unsigned aLODLevel, std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList);
		const LOD& GetLOD(unsigned aLODLevel) const;
		unsigned GetAppropriateLODLevel(float aScreenPercentage) const;

		unsigned MaterialIndex = 0;

	private:
		std::map<unsigned, LOD> myLODs;
	};

	struct Skeleton
	{
		struct Joint
		{
			Math::Matrix4x4<float> BindPoseInverse;
			int Parent = 0;
			std::vector<unsigned> Children;
			std::string Name;
		};

		std::vector<Joint> myJoints;
		std::unordered_map<std::string, size_t> JointNameToIndex;
	};

	Mesh();
	~Mesh();

	void AddSubmesh(Submesh&& aSubmesh);
	void AddLOD(unsigned aSubmeshIndex, std::vector<Vertex>&& aVertexList, std::vector<unsigned>&& aIndexList);
	void SetSkeleton(Skeleton&& aSkeleton);
	void InitBoundingBox(Math::Vector3f aMinPoint, Math::Vector3f aMaxPoint);

	FORCEINLINE const std::vector<Submesh>& GetSubmeshes() const { return mySubmeshes; }
	FORCEINLINE const Skeleton& GetSkeleton() const { return mySkeleton; }
	FORCEINLINE const Math::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

private:
	std::vector<Submesh> mySubmeshes;
	Skeleton mySkeleton;
	Math::AABB3D<float> myBoundingBox;
};