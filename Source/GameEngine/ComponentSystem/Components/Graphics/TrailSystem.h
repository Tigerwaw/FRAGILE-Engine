#pragma once
#include "ComponentSystem/Component.h"
#include "Objects/ParticleSystem/TrailEmitter.h"
#include "Math/AABB3D.hpp"

class TrailSystem : public Component
{
public:
	~TrailSystem() override;
	TrailSystem() = default;

	void Start() override;
	void Update() override;

	void ResetEmitters();

	TrailEmitter& AddEmitter(const TrailEmitterSettings& aSettings);

	std::vector<TrailEmitter>& GetEmitters() { return myEmitters; }
	
	// This bounding box is actually in world space due to how the trail works.
	const Math::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	void UpdateBoundingBox();

	std::vector<TrailEmitter> myEmitters;
	Math::AABB3D<float> myBoundingBox;
};

