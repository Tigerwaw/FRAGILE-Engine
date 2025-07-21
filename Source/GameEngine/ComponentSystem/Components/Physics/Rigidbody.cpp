#include "Enginepch.h"
#include "Rigidbody.h"
#include "Time/Timer.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/GameObject.h"

Rigidbody::Rigidbody(float aMass, bool aAffectedByGravity)
{
	SetMass(aMass);
	SetIsAffectedByGravity(aAffectedByGravity);
}

void Rigidbody::Start()
{
}

void Rigidbody::ApplyForce(Math::Vector3f aForce)
{
	myForce += aForce;
}

void Rigidbody::ApplyImpulse(Math::Vector3f aImpulse)
{
	myImpulse += aImpulse;
}

void Rigidbody::SetMass(float aMass)
{
	myMass = aMass;
	myInvMass = 1 / aMass;
}

void Rigidbody::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();
	myVelocity += myForce * myInvMass * dt;
	myVelocity += myImpulse * myInvMass;

	if (myIsAffectedByGravity)
	{
		myVelocity.y -= myTempGravity * myMass * dt;
	}

	gameObject->GetComponent<Transform>()->AddTranslation(myVelocity * dt);
	myForce = { 0.0f, 0.0f, 0.0f };
	myImpulse = { 0.0f, 0.0f, 0.0f };
	myVelocity *= 1.0f - myTempFriction;
}
