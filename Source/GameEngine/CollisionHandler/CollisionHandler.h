#pragma once
#include "ComponentSystem/Components/Physics/Colliders/Collider.h"

class Scene;

class CollisionHandler
{
public:
    struct Collision
    {
        bool isTrigger = false;
        std::shared_ptr<Collider> colliderOne;
        std::shared_ptr<Collider> colliderTwo;

        bool operator==(const Collision& aOtherCollision)
        {
            return (colliderOne == aOtherCollision.colliderOne && colliderTwo == aOtherCollision.colliderTwo)
                || (colliderOne == aOtherCollision.colliderTwo && colliderTwo == aOtherCollision.colliderOne);
        }
    };

    CollisionHandler();
    ~CollisionHandler();

    void UpdateCollisions(Scene& aScene);
    bool Raycast(Scene& aScene, Math::Vector3f aOrigin, Math::Vector3f aDirection, Math::Vector3f& aHitPoint);

private:
    void AddActiveCollisions(Scene& aScene);
    void CompareCollisions();

    std::vector<Collision> myActiveCollisionsThisFrame;
    std::vector<Collision> myActiveCollisionsLastFrame;
};

