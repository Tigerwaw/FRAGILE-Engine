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

			if (colliderA->CheckOverlap(colliderB.get()))
			{
				auto& newCollision = myActiveCollisionsThisFrame.emplace_back();
				newCollision.colliderOne = colliderA;
				newCollision.colliderTwo = colliderB;
				newCollision.isTrigger = colliderA->IsTrigger() || colliderB->IsTrigger();
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
					activeCollisionThisFrame.colliderOne->OnTriggerEnter();

				if (activeCollisionThisFrame.colliderTwo->IsTrigger())
					activeCollisionThisFrame.colliderTwo->OnTriggerEnter();
			}
			else
			{
				if (activeCollisionThisFrame.colliderOne->IsTrigger())
					activeCollisionThisFrame.colliderOne->OnTriggerStay();

				if (activeCollisionThisFrame.colliderTwo->IsTrigger())
					activeCollisionThisFrame.colliderTwo->OnTriggerStay();
			}
		}
		else
		{
			if (isNewCollision)
			{
				activeCollisionThisFrame.colliderOne->OnCollisionEnter();
				activeCollisionThisFrame.colliderTwo->OnCollisionEnter();
			}
			else
			{
				activeCollisionThisFrame.colliderOne->OnCollisionStay();
				activeCollisionThisFrame.colliderTwo->OnCollisionStay();
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
					activeCollisionLastFrame.colliderOne->OnTriggerExit();

				if (activeCollisionLastFrame.colliderTwo->IsTrigger())
					activeCollisionLastFrame.colliderTwo->OnTriggerExit();
			}
			else
			{
				activeCollisionLastFrame.colliderOne->OnCollisionExit();
				activeCollisionLastFrame.colliderTwo->OnCollisionExit();
			}
		}
	}
}

void CollisionHandler::ResolveCollision(const Collision& aCollision)
{
	auto rbOne = aCollision.colliderOne->gameObject->GetComponent<Rigidbody>();
	auto rbTwo = aCollision.colliderTwo->gameObject->GetComponent<Rigidbody>();

	if (rbOne && rbTwo)
	{
		Math::Vector3f impulseOne = rbTwo->GetVelocity() - rbOne->GetVelocity();
		Math::Vector3f impulseTwo = rbOne->GetVelocity() - rbTwo->GetVelocity();

		rbOne->ApplyImpulse(impulseOne);
		rbTwo->ApplyImpulse(impulseTwo);
	}
	else if (rbOne)
	{
		rbOne->ApplyImpulse(aCollision.overlap * 100.0f);
	}
	else if (rbTwo)
	{
		rbTwo->ApplyImpulse(aCollision.overlap * 100.0f);
	}
}