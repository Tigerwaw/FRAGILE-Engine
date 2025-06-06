#include "Enginepch.h"
#include "StateMachineController.h"
#include "GameEngine/Engine.h"
#include "GameEngine/Time/Timer.h"
#include "GameEngine/SceneHandler/SceneHandler.h"
#include "GameEngine/ComponentSystem/GameObject.h"
#include "GameEngine/ComponentSystem/Components/Transform.h"
#include "GameEngine/ComponentSystem/Components/Graphics/ParticleSystem.h"
#include "../PollingStation.h"
#include "DecisionMaking/HealthComponent.h"
#include "Math/Intersection3D.hpp"

void StateMachineController::Start()
{
}

void StateMachineController::Update()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();
	myCurrentParticleActiveTime += dt;
	if (myIsShooting && myCurrentParticleActiveTime > myMaxParticleActiveTime)
	{
		gameObject->GetComponent<ParticleSystem>()->SetActive(false);
		myIsShooting = false;
	}

	auto healthComp = gameObject->GetComponent<HealthComponent>();
	if (healthComp->GetHealth() <= 0)
	{
		myCurrentState = State::Death;
	}
	else if (healthComp->GetHealth() / healthComp->GetMaxHealth() < 0.5f)
	{
		myCurrentState = State::SeekWell;
	}

	switch (myCurrentState)
	{
	case StateMachineController::State::SeekEnemy:
	{
		SetTarget(PollingStation::Get().GetRandomAIActor(Engine::Get().GetSceneHandler().FindGameObjectByName(gameObject->GetName())));
		SeekTarget();

		auto transform = gameObject->GetComponent<Transform>();
		Math::Vector3f pos = transform->GetTranslation();
		Math::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
		Math::Vector3f directionToTarget = targetPos - pos;
		if (directionToTarget.LengthSqr() < myShootingRange * myShootingRange)
		{
			float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
			if (dot >= mySightAngle)
			{
				if (IsLineOfSightClear(pos, directionToTarget))
				{
					myCurrentState = State::Aim;
				}
			}
		}

		break;
	}
	case StateMachineController::State::SeekWell:
	{
		SetTarget(Engine::Get().GetSceneHandler().FindGameObjectByName("HWell"));
		SeekTarget();

		auto transform = gameObject->GetComponent<Transform>();
		Math::Vector3f pos = transform->GetTranslation();
		Math::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
		Math::Vector3f directionToTarget = targetPos - pos;
		if (directionToTarget.LengthSqr() >= myHealRadius * myHealRadius)
		{
			myCurrentState = State::Heal;
		}

		break;
	}
	case StateMachineController::State::Aim:
	{
		auto transform = gameObject->GetComponent<Transform>();
		Math::Vector3f pos = transform->GetTranslation();
		Math::Vector3f targetPos = myTarget->GetComponent<Transform>()->GetTranslation();
		Math::Vector3f directionToTarget = targetPos - pos;
		if (directionToTarget.LengthSqr() > myShootingRange * myShootingRange)
		{
			myCurrentState = State::SeekEnemy;
		}

		if (!IsLineOfSightClear(pos, directionToTarget))
		{
			myCurrentState = State::SeekEnemy;
		}

		myCurrentRotationTime += dt;
		if (myCurrentRotationTime >= myMaxRotationTime)
		{
			myCurrentRotationTime = 0;
			myCurrentRot = myGoalRot;
			myGoalRot = Math::Quatf(Math::Vector3f(0, std::atan2(directionToTarget.x, directionToTarget.z), 0));
		}

		float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
		Math::Quatf rot = Math::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
		transform->SetRotation(rot.GetEulerAnglesDegrees());
		float dot = transform->GetForwardVector().Dot(directionToTarget.GetNormalized());
		myTimeSinceLastShot += dt;
		if (dot >= myShootingAngle)
		{
			if (myTimeSinceLastShot > myShootingCooldown)
			{
				myTimeSinceLastShot = 0;
				if (auto targetHealthComp = myTarget->GetComponent<HealthComponent>())
				{
					targetHealthComp->TakeDamage(myDamage);

					if (targetHealthComp->GetHealth() <= 0)
					{
						SetTarget(PollingStation::Get().GetRandomAIActor(Engine::Get().GetSceneHandler().FindGameObjectByName(gameObject->GetName())));
					}
				}

				gameObject->GetComponent<ParticleSystem>()->SetActive(true);
				myCurrentParticleActiveTime = 0;
				myIsShooting = true;
			}
		}
		else if (dot < mySightAngle)
		{
			myCurrentState = State::SeekEnemy;
		}
		break;
	}
	case StateMachineController::State::Heal:
	{
		healthComp->Heal(myHPS * dt);
		if (healthComp->GetHealth() / healthComp->GetMaxHealth() > 0.9f)
		{
			myCurrentState = State::SeekEnemy;
		}
		break;
	}
	case StateMachineController::State::Death:
	{
		myCurrentDeathTime += dt;

		if (myCurrentDeathTime >= myDeathTimer)
		{
			myCurrentDeathTime = 0.0f;
			auto transform = gameObject->GetComponent<Transform>();
			transform->SetTranslation(300.0f, 0, -500.0f);
			healthComp->Heal(healthComp->GetMaxHealth());
			myCurrentState = State::SeekEnemy;
		}
		break;
	}
	default:
		break;
	}
}

void StateMachineController::SeekTarget()
{
	float dt = Engine::Get().GetTimer().GetDeltaTime();
	auto& transform = gameObject->GetComponent<Transform>();

	Math::Vector3f pos = transform->GetTranslation();
	Math::Vector3f velocity;

	// Seek Target
	{
		velocity += myTarget->GetComponent<Transform>()->GetTranslation() - pos;
	}

	// Avoid walls
	{
		Math::Vector3f avoidVelocity;
		for (auto& wallPos : PollingStation::Get().GetWallPositions())
		{
			Math::Vector3f diff = pos - wallPos;
			if (diff.LengthSqr() > myAvoidRadius * myAvoidRadius) continue;
			avoidVelocity += diff;
		}

		avoidVelocity *= myAvoidFactor;
		velocity += avoidVelocity;
	}

	// Clamp steering velocity to max acceleration
	if (velocity.LengthSqr() > myMaxAcceleration * myMaxAcceleration)
	{
		velocity.Normalize();
		velocity *= myMaxAcceleration;
	}

	myVelocity += velocity;
	transform->AddTranslation(myVelocity * dt);

	if (myVelocity.LengthSqr() > 1.0f)
	{
		myCurrentRotationTime += dt;
		if (myCurrentRotationTime >= myMaxRotationTime)
		{
			myCurrentRotationTime = 0;
			myCurrentRot = myGoalRot;
			myGoalRot = Math::Quatf(Math::Vector3f(0, std::atan2(myVelocity.x, myVelocity.z), 0));
		}

		float rotTimeDelta = myCurrentRotationTime / myMaxRotationTime;
		Math::Quatf rot = Math::Quatf::Slerp(myCurrentRot, myGoalRot, rotTimeDelta);
		gameObject->GetComponent<Transform>()->SetRotation(rot.GetEulerAnglesDegrees());
	}

	myVelocity = myVelocity * (1 - myDeceleration);
}

bool StateMachineController::IsLineOfSightClear(Math::Vector3f aOrigin, Math::Vector3f aDiff)
{
	for (auto& wallPos : PollingStation::Get().GetWallPositions())
	{
		if ((wallPos - aOrigin).LengthSqr() > aDiff.LengthSqr()) continue;

		Math::Sphere<float> sphere(wallPos, myAvoidRadius);
		Math::Ray<float> ray(aOrigin, aDiff.GetNormalized());

		if (Math::IntersectionSphereRay(sphere, ray))
		{
			return false;
		}
	}

	return true;
}
