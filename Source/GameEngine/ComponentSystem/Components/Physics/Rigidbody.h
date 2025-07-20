#pragma once
#include "ComponentSystem/Component.h"

class Rigidbody : public Component
{
public:
	Rigidbody() = default;
	Rigidbody(float aMass);
	void Start() override;
	void Update() override;
	void ApplyForce(Math::Vector3f aForce);

private:
	Math::Vector3f myVelocity;
	Math::Vector3f myForce;
	float myMass;
};