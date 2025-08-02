#pragma once
#include "Collider.h"
#include "Math/AABB3D.hpp"

class SphereCollider;

class BoxCollider : public Collider
{
public:
    BoxCollider(bool aIsTrigger = false, Math::Vector3f aExtents = Math::Vector3f(1.0f, 1.0f, 1.0f), Math::Vector3f aCenterOffset = Math::Vector3f());

    CollisionInfo CheckOverlap(const Collider* aCollider) const override;
    CollisionInfo CheckOverlap(const BoxCollider* aCollider) const override;
    CollisionInfo CheckOverlap(const SphereCollider* aCollider) const override;
    CollisionInfo CheckOverlap(const Math::Ray<float> aRay) const override;

    const Math::AABB3D<float>& GetAABB() const;

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
protected:
    Math::AABB3D<float> myAABB;
};

