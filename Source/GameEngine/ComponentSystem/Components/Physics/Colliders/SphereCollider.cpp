#include "Enginepch.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "Math/Intersection3D.hpp"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/Components/Transform.h"
#include "CommonUtilities/SerializationUtils.hpp"

SphereCollider::SphereCollider(float aRadius, Math::Vector3f aCenterOffset)
{
    mySphere.InitWithCenterAndRadius(aCenterOffset, aRadius);
}

Collider::CollisionInfo SphereCollider::CheckOverlap(const Collider* aCollider) const
{
    return aCollider->CheckOverlap(this);
}

Collider::CollisionInfo SphereCollider::CheckOverlap(const BoxCollider* aCollider) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    std::shared_ptr<Transform> collTransform = aCollider->gameObject->GetComponent<Transform>();
    Math::AABB3D<float> aabbInMySpace = aCollider->GetAABB().GetAABBinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrixInverse());

    return Math::IntersectionSphereAABB(GetSphere(), aabbInMySpace);
}

Collider::CollisionInfo SphereCollider::CheckOverlap(const SphereCollider* aCollider) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    std::shared_ptr<Transform> collTransform = aCollider->gameObject->GetComponent<Transform>();
    Math::Sphere<float> otherSphereInMySpace = aCollider->GetSphere().GetSphereinNewSpace(collTransform->GetWorldMatrix() * transform->GetWorldMatrixInverse());

    return Math::IntersectionBetweenSpheres(GetSphere(), otherSphereInMySpace);
}

Collider::CollisionInfo SphereCollider::CheckOverlap(const Math::Ray<float> aRay) const
{
    std::shared_ptr<Transform> transform = gameObject->GetComponent<Transform>();
    Math::Ray<float> rayInMySpace = aRay.GetRayinNewSpace(transform->GetWorldMatrixInverse());
    
    return Math::IntersectionSphereRay(GetSphere(), rayInMySpace);
}

const Math::Sphere<float>& SphereCollider::GetSphere() const
{
    return mySphere;
}

bool SphereCollider::Serialize(nl::json& outJsonObject)
{
    outJsonObject;
    return false;
}

bool SphereCollider::Deserialize(nl::json& aJsonObject)
{
    Collider::Deserialize(aJsonObject);

    Math::Vector3f centerOffset;
    float radius = 0;

    if (aJsonObject.contains("CenterOffset"))
    {
        centerOffset = Utilities::DeserializeVector3<float>(aJsonObject["CenterOffset"]);
    }

    if (aJsonObject.contains("Radius"))
    {
        radius = aJsonObject["Radius"].get<float>();
    }

    mySphere.InitWithCenterAndRadius(centerOffset, radius);
    return true;
}
