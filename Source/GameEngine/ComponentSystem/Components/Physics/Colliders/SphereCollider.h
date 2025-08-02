#pragma once
#include "Collider.h"
#include "Math/Sphere.hpp"

class BoxCollider;

class SphereCollider : public Collider
{
public:
    SphereCollider(bool aIsTrigger = false, float aRadius = 100.0f, Math::Vector3f aCenterOffset = Math::Vector3f());

    CollisionInfo CheckOverlap(const Collider* aCollider) const override;
    CollisionInfo CheckOverlap(const BoxCollider* aCollider) const override;
    CollisionInfo CheckOverlap(const SphereCollider* aCollider) const override;
    CollisionInfo CheckOverlap(const Math::Ray<float> aRay) const override;

    const Math::Sphere<float>& GetSphere() const;

    bool Serialize(nl::json& outJsonObject) override;
    bool Deserialize(nl::json& aJsonObject) override;
protected:
    Math::Sphere<float> mySphere;
};