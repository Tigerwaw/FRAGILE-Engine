#include "Enginepch.h"
#include "Rigidbody.h"
#include "Time/Timer.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/GameObject.h"

Rigidbody::Rigidbody(float aMass)
{
	myMass = aMass;
}

void Rigidbody::Start()
{
}

void Rigidbody::ApplyForce(Math::Vector3f aForce)
{
	myForce += aForce;
}

void Rigidbody::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();
	myVelocity += myForce / myMass * dt;
	gameObject->GetComponent<Transform>()->AddTranslation(myVelocity * dt);
	myForce = { 0.0f, 0.0f, 0.0f };
}
