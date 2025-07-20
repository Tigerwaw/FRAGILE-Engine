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
	void ApplyImpulse(Math::Vector3f aImpulse);

	Math::Vector3f GetVelocity() const { return myVelocity; }
	void SetMass(float aMass);
	float GetMass() const { return myMass; }
	float GetInvMass() const { return myInvMass; }

private:
	Math::Vector3f myVelocity;
	Math::Vector3f myForce;
	float myMass;
	float myInvMass;

	float myTempFriction = 0.01f;
};