#pragma once
#include "Objects/Vertices/TrailVertex.h"
#include "TrailEmitterSettings.h"

#include "Math/AABB3D.hpp"

class DynamicVertexBuffer;

class TrailEmitter
{
	friend class Drawer;
	friend class TrailSystem;

public:
	TrailEmitter();
	virtual ~TrailEmitter();
	void Update(Math::Vector3f aFollowTarget, float aDeltaTime);

	void SetMaterial(std::shared_ptr<Material> aMaterial) { myMaterial = aMaterial; }
	const std::shared_ptr<Material> GetMaterial() const { return myMaterial; }

	TrailEmitterSettings& GetEmitterSettings() { return mySettings; }
	const unsigned GetCurrentLength() const { return myCurrentLength; }

	void ResetTrail();

	const Math::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

protected:
	virtual void InitTrailVertex(TrailVertex& aTrailVertex);
	virtual void UpdateTrailVertex(TrailVertex& aTrailVertex, float aDeltaTime, unsigned aIndex);
	virtual void InitInternal();
	std::vector<TrailVertex> myTrailVertices;
private:
	void UpdateBoundingBox(const Math::Vector3f& aPosition, float aWidth);

	std::shared_ptr<DynamicVertexBuffer> myVertexBuffer;
	std::shared_ptr<Material> myMaterial;
	TrailEmitterSettings mySettings;
	std::vector<Math::Vector3f> myPreviousPositions;
	unsigned myCurrentLength = 0;

	Math::AABB3D<float> myBoundingBox;
};

