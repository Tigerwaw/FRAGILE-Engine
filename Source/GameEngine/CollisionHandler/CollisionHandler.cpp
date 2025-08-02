#include "Enginepch.h"
#include "CollisionHandler.h"
#include "ComponentSystem/Scene.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Component.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Physics/Colliders/Collider.h"
#include "ComponentSystem/Components/Physics/Rigidbody.h"
#include "Math/Intersection3D.hpp"

CollisionHandler::CollisionHandler()
{
}

CollisionHandler::~CollisionHandler()
{
}

void CollisionHandler::UpdateCollisions(Scene& aScene)
{
	PIXScopedEvent(PIX_COLOR_INDEX(9), "Update Collisions in Scene");

	AddActiveCollisions(aScene);
	CompareCollisions();
	myActiveCollisionsLastFrame = myActiveCollisionsThisFrame;
	myActiveCollisionsThisFrame.clear();
}

bool CollisionHandler::Raycast(Scene& aScene, Math::Vector3f aOrigin, Math::Vector3f aDirection, Math::Vector3f& aHitPoint)
{
	for (auto& goA : aScene.myGameObjects)
	{
		if (!goA->GetActive()) continue;

		std::shared_ptr<Collider> colliderA = goA->GetComponent<Collider>();
		if (!colliderA || !colliderA->GetActive()) continue;

		Math::Ray<float> aRay(aOrigin, aDirection);
		auto info = colliderA->CheckOverlap(aRay);
		if (info)
		{
			aHitPoint = info.hitPoint;
			return true;
		}
	}

	aHitPoint = { 0, 0, 0 };
	return false;
}

void CollisionHandler::AddActiveCollisions(Scene& aScene)
{
	for (size_t indexA = 0; indexA < aScene.myGameObjects.size() - 1; ++indexA)
	{
		std::shared_ptr<GameObject> goA = aScene.myGameObjects[indexA];
		if (!goA->GetActive()) continue;
		bool isStaticA = goA->GetStatic();

		std::shared_ptr<Collider> colliderA = goA->GetComponent<Collider>();
		if (!colliderA || !colliderA->GetActive()) continue;

		for (size_t indexB = indexA + 1; indexB < aScene.myGameObjects.size(); ++indexB)
		{
			std::shared_ptr<GameObject> goB = aScene.myGameObjects[indexB];
			if (!goB->GetActive()) continue;
			bool isStaticB = goB->GetStatic();
			if (isStaticA && isStaticB) continue;

			std::shared_ptr<Collider> colliderB = goB->GetComponent<Collider>();
			if (!colliderB || !colliderB->GetActive()) continue;

			Collider::CollisionInfo info = colliderA->CheckOverlap(colliderB.get());
			if (info.collided)
			{
				auto& newCollision = myActiveCollisionsThisFrame.emplace_back();
				newCollision.colliderOne = colliderA;
				newCollision.colliderTwo = colliderB;
				newCollision.isTrigger = colliderA->IsTrigger() || colliderB->IsTrigger();
				newCollision.info = info;
				newCollision.info.collA = colliderA.get();
				newCollision.info.collB = colliderB.get();
			}
		}
	}
}

void CollisionHandler::CompareCollisions()
{
	for (auto& activeCollisionThisFrame : myActiveCollisionsThisFrame)
	{
		auto it = std::find(myActiveCollisionsLastFrame.begin(), myActiveCollisionsLastFrame.end(), activeCollisionThisFrame);
		bool isNewCollision = (it == myActiveCollisionsLastFrame.end());
		if (activeCollisionThisFrame.isTrigger)
		{
			if (isNewCollision)
			{
				if (activeCollisionThisFrame.colliderOne->IsTrigger())
					activeCollisionThisFrame.colliderOne->OnTriggerEnter(activeCollisionThisFrame.info);

				if (activeCollisionThisFrame.colliderTwo->IsTrigger())
					activeCollisionThisFrame.colliderTwo->OnTriggerEnter(activeCollisionThisFrame.info);
			}
			else
			{
				if (activeCollisionThisFrame.colliderOne->IsTrigger())
					activeCollisionThisFrame.colliderOne->OnTriggerStay(activeCollisionThisFrame.info);

				if (activeCollisionThisFrame.colliderTwo->IsTrigger())
					activeCollisionThisFrame.colliderTwo->OnTriggerStay(activeCollisionThisFrame.info);
			}
		}
		else
		{
			if (isNewCollision)
			{
				activeCollisionThisFrame.colliderOne->OnCollisionEnter(activeCollisionThisFrame.info);
				activeCollisionThisFrame.colliderTwo->OnCollisionEnter(activeCollisionThisFrame.info);
			}
			else
			{
				activeCollisionThisFrame.colliderOne->OnCollisionStay(activeCollisionThisFrame.info);
				activeCollisionThisFrame.colliderTwo->OnCollisionStay(activeCollisionThisFrame.info);
			}

			ResolveCollision(activeCollisionThisFrame);
		}
	}

	for (auto& activeCollisionLastFrame : myActiveCollisionsLastFrame)
	{
		auto it = std::find(myActiveCollisionsThisFrame.begin(), myActiveCollisionsThisFrame.end(), activeCollisionLastFrame);
		bool collisionEnded = (it == myActiveCollisionsThisFrame.end());
		if (collisionEnded)
		{
			if (activeCollisionLastFrame.isTrigger)
			{
				if (activeCollisionLastFrame.colliderOne->IsTrigger())
					activeCollisionLastFrame.colliderOne->OnTriggerExit(activeCollisionLastFrame.info);

				if (activeCollisionLastFrame.colliderTwo->IsTrigger())
					activeCollisionLastFrame.colliderTwo->OnTriggerExit(activeCollisionLastFrame.info);
			}
			else
			{
				activeCollisionLastFrame.colliderOne->OnCollisionExit(activeCollisionLastFrame.info);
				activeCollisionLastFrame.colliderTwo->OnCollisionExit(activeCollisionLastFrame.info);
			}
		}
	}
}

void CollisionHandler::ResolveCollision(const Collision& aCollision)
{
	ResolveInterpenetration(aCollision);
	ResolveImpulse(aCollision);
}

void CollisionHandler::ResolveInterpenetration(const Collision& aCollision)
{
	auto rbOne = aCollision.colliderOne->gameObject->GetComponent<Rigidbody>();
	auto rbTwo = aCollision.colliderTwo->gameObject->GetComponent<Rigidbody>();

	Math::Vector3f correctionVector = aCollision.info.normal * aCollision.info.depth;
	if (rbOne && rbTwo)
	{
		correctionVector *= 0.5f;
	}

	if (rbOne)
	{
		aCollision.colliderOne->gameObject->GetComponent<Transform>()->AddTranslation(-correctionVector);
		rbOne->ResetVelocity();
	}

	if (rbTwo)
	{
		aCollision.colliderTwo->gameObject->GetComponent<Transform>()->AddTranslation(correctionVector);
		rbTwo->ResetVelocity();
	}
}

void CollisionHandler::ResolveImpulse(const Collision& aCollision)
{
	auto rbOne = aCollision.colliderOne->gameObject->GetComponent<Rigidbody>();
	auto rbTwo = aCollision.colliderTwo->gameObject->GetComponent<Rigidbody>();

	if (rbOne && rbTwo)
	{
		Math::Vector3f relativeVelocity = rbOne->GetVelocity() - rbTwo->GetVelocity();
		float dot = relativeVelocity.Dot(aCollision.info.normal);
		float restitution = 1.0f * 1.0f;
		float j = -(1.0f + restitution) * dot / (rbOne->GetInvMass() + rbTwo->GetInvMass());
		Math::Vector3f impulse = j * aCollision.info.normal;

		rbOne->ApplyImpulse(impulse * rbOne->GetInvMass());
		rbTwo->ApplyImpulse(-impulse * rbTwo->GetInvMass());
	}
	else if (rbOne)
	{
		rbOne->ApplyImpulse(aCollision.info.normal * aCollision.info.depth);
	}
	else if (rbTwo)
	{
		rbTwo->ApplyImpulse(-aCollision.info.normal * aCollision.info.depth);
	}
}
