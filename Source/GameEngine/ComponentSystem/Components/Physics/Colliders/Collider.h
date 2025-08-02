#pragma once
#include "ComponentSystem/Component.h"
#include "Math/Ray.hpp"
#include "Math/Intersection3D.hpp"

class BoxCollider;
class SphereCollider;
class CollisionInfo;

class Collider : public Component
{
    friend class CollisionHandler;
public:
    struct CollisionInfo
    {
        Collider* collA;
        Collider* collB;
        bool collided = false;
        Math::Vector3f hitPoint;
        Math::Vector3f normal;
        float depth;

        operator bool() const
        {
            return collided;
        }

        CollisionInfo() = default;
        CollisionInfo(const Math::IntersectionInfo& aInfo)
        {
            collided = aInfo.intersected;
            hitPoint = aInfo.intersectionPoint;
            normal = aInfo.normal;
            depth = aInfo.depth;
        }
    };

    void Start() override {}
    void Update() override {}
    void SetIsTrigger(bool aIsTrigger) { myIsTrigger = aIsTrigger; }
    bool IsTrigger() const { return myIsTrigger; }

    bool IsOverlapping() const { return myIsOverlappingDebug; }

    virtual CollisionInfo CheckOverlap(const Collider* aCollider) const = 0;
    virtual CollisionInfo CheckOverlap(const BoxCollider* aCollider) const = 0;
    virtual CollisionInfo CheckOverlap(const SphereCollider* aCollider) const = 0;
    virtual CollisionInfo CheckOverlap(const Math::Ray<float> aRay) const = 0;

    void SetOnCollisionEnterResponse(const std::function<void(CollisionInfo)>& aCallback);
    void SetOnCollisionStayResponse(const std::function<void(CollisionInfo)>& aCallback);
    void SetOnCollisionExitResponse(const std::function<void(CollisionInfo)>& aCallback);

    void SetOnTriggerEnterResponse(const std::function<void(CollisionInfo)>& aCallback);
    void SetOnTriggerStayResponse(const std::function<void(CollisionInfo)>& aCallback);
    void SetOnTriggerExitResponse(const std::function<void(CollisionInfo)>& aCallback);

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;

private:
    bool myIsTrigger = false;
    bool myIsOverlappingDebug = false;

    void OnCollisionEnter(CollisionInfo aCollision);
    void OnCollisionStay(CollisionInfo aCollision);
    void OnCollisionExit(CollisionInfo aCollision);

    void OnTriggerEnter(CollisionInfo aCollision);
    void OnTriggerStay(CollisionInfo aCollision);
    void OnTriggerExit(CollisionInfo aCollision);
    std::function<void(CollisionInfo)> myOnCollisionEnterResponse;
    std::function<void(CollisionInfo)> myOnCollisionStayResponse;
    std::function<void(CollisionInfo)> myOnCollisionExitResponse;
    std::function<void(CollisionInfo)> myOnTriggerEnterResponse;
    std::function<void(CollisionInfo)> myOnTriggerStayResponse;
    std::function<void(CollisionInfo)> myOnTriggerExitResponse;
};

