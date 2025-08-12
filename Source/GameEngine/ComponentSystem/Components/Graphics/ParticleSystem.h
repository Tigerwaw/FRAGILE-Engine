#pragma once
#include "ComponentSystem/Component.h"
#include "Objects/ParticleSystem/ParticleEmitter.h"
#include "Math/AABB3D.hpp"

class ParticleSystem : public Component
{
public:
	~ParticleSystem() override;
	ParticleSystem() = default;

	void Start() override;
	void Update() override;

	void ResetEmitters();

	ParticleEmitter& AddEmitter(const ParticleEmitterSettings& aSettings);

	std::vector<ParticleEmitter>& GetEmitters() { return myEmitters; }

	const Math::AABB3D<float>& GetBoundingBox() const { return myBoundingBox; }

	bool Serialize(nl::json& outJsonObject) override;
	bool Deserialize(nl::json& aJsonObject) override;
private:
	void UpdateBoundingBox();

	std::vector<ParticleEmitter> myEmitters;
	Math::AABB3D<float> myBoundingBox;
};
