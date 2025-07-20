#pragma once
#include <vector>
#include "Math/Vector3.hpp"
#include "Plane.hpp"
#include "Sphere.hpp"
#include "AABB3D.hpp"
#include "Ray.hpp"
#include "PlaneVolume.hpp"

#undef min
#undef max

namespace Math
{
	struct IntersectionInfo
	{
		bool intersected = false;
		Vector3f intersectionPoint;
		Vector3f overlap;

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
			Vector3<T> centerDelta = Vector3<T>::Abs(aAABB3D.GetCenter() - aSphere.GetPoint());
			Vector3<T> aabbExtents = aAABB3D.GetExtents();
			Vector3<T> overlap;
			overlap.x = aabbExtents.x + aSphere.GetRadius() - centerDelta.x;
			overlap.y = aabbExtents.y + aSphere.GetRadius() - centerDelta.y;
			overlap.z = aabbExtents.z + aSphere.GetRadius() - centerDelta.z;

			IntersectionInfo info;
			info.intersected = true;
			info.intersectionPoint = closestPoint;
			info.overlap = overlap;
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
			Vector3<T> centerDelta = Vector3<T>::Abs(aBoundingBoxOne.GetCenter() - aBoundingBoxTwo.GetCenter());
			Vector3<T> aabbOneExtents = aBoundingBoxOne.GetExtents();
			Vector3<T> aabbTwoExtents = aBoundingBoxTwo.GetExtents();
			Vector3<T> overlap = aabbOneExtents + aabbTwoExtents - centerDelta;

			IntersectionInfo info;
			info.intersected = true;
			info.overlap = overlap;
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
			Vector3<T> centerDelta = Vector3<T>::Abs(aSphereOne.GetPoint() - aSphereTwo.GetPoint());
			Vector3<T> radiusSumAsVector = { radiusSum, radiusSum, radiusSum };
			Vector3<T> overlap = radiusSumAsVector - centerDelta;

			IntersectionInfo info;
			info.intersected = true;
			info.overlap = overlap;
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
}