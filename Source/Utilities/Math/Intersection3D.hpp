#pragma once
#include <vector>
#include "Math/Vector3.hpp"
#include "Plane.hpp"
#include "Sphere.hpp"
#include "AABB3D.hpp"
#include "Ray.hpp"
#include "PlaneVolume.hpp"
#include "ArithmeticUtilities.hpp"

#undef min
#undef max

namespace Math
{
	struct IntersectionInfo
	{
		bool intersected = false;
		Vector3f intersectionPoint;
		Vector3f normal;
		float depth;

		operator bool() const
		{
			return intersected;
		}
	};

	template <class T>
	IntersectionInfo IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay)
	{
		T tolerance = (T)0.0001;
		T d = aPlane.GetPoint().Dot(aPlane.GetNormal());
		T numerator = d - aRay.GetOrigin().Dot(aPlane.GetNormal());
		T denominator = aRay.GetDirection().Dot(aPlane.GetNormal());

		// Ray is parallel to the plane
		if (denominator < tolerance && denominator > -tolerance)
		{
			// Ray Origin is on the plane
			//if (numerator < tolerance && numerator > -tolerance)
			//{
			//	return true;
			//}

			return IntersectionInfo();
		}

		T t = numerator / denominator;

		// Ray is pointing away from the plane
		if (t < 0)
		{
			return IntersectionInfo();
		}

		IntersectionInfo info;
		info.intersected = true;
		info.intersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * t;
		return info;
	}

	template <class T>
	IntersectionInfo IntersectionAABBRay(const AABB3D<T>& aAABB, const Ray<T>& aRay)
	{
		const Vector3<T> rDir = aRay.GetDirection();

		const Vector3<T> originToMin = (aAABB.GetMin() - aRay.GetOrigin());
		const Vector3<T> originToMax = (aAABB.GetMax() - aRay.GetOrigin());

		const Vector3<T> t1{ originToMin.x / rDir.x, originToMin.y / rDir.y, originToMin.z / rDir.z };
		const Vector3<T> t2{ originToMax.x / rDir.x, originToMax.y / rDir.y, originToMax.z / rDir.z };

		T tMin = 0;
		T tMax = static_cast<T>(FLT_MAX);

		tMin = static_cast<T>(std::fmin(std::fmax(t1.x, tMin), std::fmax(t2.x, tMin)));
		tMax = static_cast<T>(std::fmax(std::fmin(t1.x, tMax), std::fmin(t2.x, tMax)));

		tMin = static_cast<T>(std::fmin(std::fmax(t1.y, tMin), std::fmax(t2.y, tMin)));
		tMax = static_cast<T>(std::fmax(std::fmin(t1.y, tMax), std::fmin(t2.y, tMax)));

		tMin = static_cast<T>(std::fmin(std::fmax(t1.z, tMin), std::fmax(t2.z, tMin)));
		tMax = static_cast<T>(std::fmax(std::fmin(t1.z, tMax), std::fmin(t2.z, tMax)));

		if (tMin <= tMax)
		{
			tMin = static_cast<T>(std::fmax(tMin, T(0))); // clamp tMin to zero if inside aabb

			IntersectionInfo info;
			info.intersected = true;
			info.intersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * tMin;
			return info;
		}

		return IntersectionInfo();
	}

	template <class T>
	IntersectionInfo IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay)
	{
		Vector3<T> sphereCenterToRayPoint = aSphere.GetPoint() - aRay.GetOrigin();
		T distanceProjectedOntoRay = sphereCenterToRayPoint.Dot(aRay.GetDirection());

		if (distanceProjectedOntoRay < 0)
		{
			return IntersectionInfo();
		}

		if (0 > aSphere.GetRadiusSqr() - sphereCenterToRayPoint.LengthSqr() + (distanceProjectedOntoRay * distanceProjectedOntoRay))
		{
			return IntersectionInfo();
		}

		IntersectionInfo info;
		info.intersected = true;
		info.intersectionPoint = aRay.GetOrigin() + aRay.GetDirection() * distanceProjectedOntoRay;
		return info;
	}

	template <class T>
	IntersectionInfo IntersectionSphereAABB(const Sphere<T>& aSphere, const AABB3D<T>& aAABB3D)
	{
		Vector3<T> closestPoint = aSphere.GetPoint();

		if (aSphere.GetPoint().x <= aAABB3D.GetMin().x)
		{
			closestPoint.x = aAABB3D.GetMin().x;
		}
		else if (aSphere.GetPoint().x >= aAABB3D.GetMax().x)
		{
			closestPoint.x = aAABB3D.GetMax().x;
		}

		if (aSphere.GetPoint().y <= aAABB3D.GetMin().y)
		{
			closestPoint.y = aAABB3D.GetMin().y;
		}
		else if (aSphere.GetPoint().y >= aAABB3D.GetMax().y)
		{
			closestPoint.y = aAABB3D.GetMax().y;
		}

		if (aSphere.GetPoint().z <= aAABB3D.GetMin().z)
		{
			closestPoint.z = aAABB3D.GetMin().z;
		}
		else if (aSphere.GetPoint().z >= aAABB3D.GetMax().z)
		{
			closestPoint.z = aAABB3D.GetMax().z;
		}

		if (aSphere.IsInside(closestPoint))
		{
			Vector3<T> sphereCenterToClosestPoint = closestPoint - aSphere.GetPoint();
			Vector3<T> furthestPointOfSphereInAABB = sphereCenterToClosestPoint.GetNormalized() * aSphere.GetRadius();
			Vector3<T> overlap = sphereCenterToClosestPoint - furthestPointOfSphereInAABB;

			IntersectionInfo info;
			info.intersectionPoint = closestPoint;
			info.intersected = true;
			info.normal = overlap.GetNormalized();
			info.depth = overlap.Length();
			return info;
		}

		return IntersectionInfo();
	}

	// Needs Testing
	template <class T>
	IntersectionInfo IntersectionSpherePlane(const Sphere<T>& aSphere, const Plane<T>& aPlane)
	{
		Vector3f closestPointOnPlane = aPlane.ClosestPointOnPlane(aSphere.GetPoint());
		Vector3f distanceToSphere = aSphere.GetPoint() - closestPointOnPlane;
		if (distanceToSphere.LengthSqr() <= aSphere.GetRadiusSqr())
		{
			IntersectionInfo info;
			info.intersected = true;
			info.intersectionPoint = closestPointOnPlane;
			info.normal = distanceToSphere.GetNormalized();
			info.depth = distanceToSphere.Length();
			return info;
		}

		return IntersectionInfo();
	}

	// Needs Testing
	template <class T>
	IntersectionInfo IntersectionAABBPlane(const AABB3D<T>& aAABB, const Plane<T>& aPlane)
	{
		Vector3<T> closestPointOnPlane = aPlane.ClosestPointOnPlane(aAABB.GetCenter());
		Vector3<T> distanceToAABB = aAABB.GetCenter() - closestPointOnPlane;
		if (aAABB.IsInside(closestPointOnPlane))
		{
			IntersectionInfo info;
			info.intersected = true;
			info.intersectionPoint = closestPointOnPlane;
			info.normal = distanceToAABB.GetNormalized();
			info.depth = distanceToAABB.Length();
			return info;
		}

		return IntersectionInfo();
	}

	template<class T>
	IntersectionInfo IntersectionBetweenAABBS(const AABB3D<T>& aBoundingBoxOne, const AABB3D<T>& aBoundingBoxTwo)
	{
		Vector3<T> minA = aBoundingBoxOne.GetMin();
		Vector3<T> maxA = aBoundingBoxOne.GetMax();
		Vector3<T> minB = aBoundingBoxTwo.GetMin();
		Vector3<T> maxB = aBoundingBoxTwo.GetMax();
		bool x = minA.x <= maxB.x && maxA.x >= minB.x;
		bool y = minA.y <= maxB.y && maxA.y >= minB.y;
		bool z = minA.z <= maxB.z && maxA.z >= minB.z;

		if (x && y && z)
		{
			Vector3<T> centerDelta = aBoundingBoxOne.GetCenter() - aBoundingBoxTwo.GetCenter();
			Vector3<T> combinedExtents = aBoundingBoxOne.GetExtents() + aBoundingBoxTwo.GetExtents();
			Vector3<T> overlap = combinedExtents - Vector3<T>::Abs(centerDelta);

			T minOverlap = std::min(overlap.x, std::min(overlap.y, overlap.z));
			Vector3<T> correctionVector;
			if (Equal(minOverlap, overlap.x, static_cast<T>(0.01)))
			{
				correctionVector.x = Sign(centerDelta.x) * overlap.x;
			}
			else if (Equal(minOverlap, overlap.z, static_cast<T>(0.01)))
			{
				correctionVector.z = Sign(centerDelta.z) * overlap.z;
			}
			else
			{
				correctionVector.y = Sign(centerDelta.y) * overlap.y;
			}

			IntersectionInfo info;
			info.intersected = true;
			info.normal = correctionVector.GetNormalized();
			info.depth = correctionVector.Length();
			return info;
		}
		
		return IntersectionInfo();
	}

	template<class T>
	IntersectionInfo IntersectionBetweenSpheres(const Sphere<T>& aSphereOne, const Sphere<T>& aSphereTwo)
	{
		Vector3<T> differenceFromCenterToCenter = aSphereOne.GetPoint() - aSphereTwo.GetPoint();
		T distanceSqr = differenceFromCenterToCenter.LengthSqr();
		T radiusSum = aSphereOne.GetRadius() + aSphereTwo.GetRadius();
		T sqrdRadii = radiusSum * radiusSum;

		if (distanceSqr <= sqrdRadii)
		{
			Vector3<T> centerDelta = aSphereOne.GetPoint() - aSphereTwo.GetPoint();
			Vector3<T> radiusOneAsVector = { aSphereOne.GetRadius(), aSphereOne.GetRadius(), aSphereOne.GetRadius() };
			Vector3<T> radiusTwoAsVector = { aSphereTwo.GetRadius(), aSphereTwo.GetRadius(), aSphereTwo.GetRadius() };
			Vector3<T> radiusSumAsVector = { radiusSum, radiusSum, radiusSum };
			Vector3<T> overlap = radiusSumAsVector - Vector3<T>::Abs(centerDelta);

			IntersectionInfo info;
			info.intersected = true;
			info.normal = overlap.GetNormalized();
			info.depth = overlap.Length();
			return info;
		}
		
		return IntersectionInfo();
	}

	template<class T>
	IntersectionInfo IntersectionBetweenPlaneVolumeAABB(const PlaneVolume<T>& aPlaneVolume, const AABB3D<T>& aBoundingBox)
	{
		for (const Plane<T>& plane : aPlaneVolume.GetPlanes())
		{
			int cornersInPlane = 0;

			for (Vector3<T> corner : aBoundingBox.GetCorners())
			{
				if (plane.IsInside(corner))
				{
					cornersInPlane++;
				}
			}

			if (cornersInPlane == 0) return IntersectionInfo();
		}

		IntersectionInfo info;
		info.intersected = true;
		return info;
	}

	template<class T>
	IntersectionInfo IntersectionBetweenPlaneVolumeSphere(const PlaneVolume<T>& aPlaneVolume, const Sphere<T>& aSphere)
	{
		for (const Plane<T>& plane : aPlaneVolume.GetPlanes())
		{
			if (!IntersectionSpherePlane(aSphere, plane))
				return IntersectionInfo();
		}

		IntersectionInfo info;
		info.intersected = true;
		return info;
	}
}