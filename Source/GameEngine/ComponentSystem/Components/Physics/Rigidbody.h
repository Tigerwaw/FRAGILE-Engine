#pragma once
#include "ComponentSystem/Component.h"

class Rigidbody : public Component
{
public:
	Rigidbody() = default;
	Rigidbody(float aMass, bool aAffectedByGravity = true);
	void Start() override;
	void Update() override;
	void ApplyForce(Math::Vector3f aForce);
	void ApplyImpulse(Math::Vector3f aImpulse);
	void ResetVelocity() { myVelocity = { 0.0f, 0.0f, 0.0f }; }

	Math::Vector3f GetVelocity() const { return myVelocity; }
	void SetMass(float aMass);
	float GetMass() const { return myMass; }
	float GetInvMass() const { return myInvMass; }

	void SetIsAffectedByGravity(bool aToggle) { myIsAffectedByGravity = aToggle; }
	bool GetIsAffectedByGravity() const { return myIsAffectedByGravity; }

private:
	bool myIsAffectedByGravity = true;

	Math::Vector3f myVelocity;
	Math::Vector3f myForce;
	Math::Vector3f myImpulse;
	float myMass;
	float myInvMass;

	float myTempFriction = 0.01f;
	float myTempGravity = 1000.0f;
};