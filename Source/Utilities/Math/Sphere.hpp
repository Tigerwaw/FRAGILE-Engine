#pragma once
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

namespace Math
{
#undef min
#undef max

	template <class T>
	class Sphere
	{
	public:
		// Default constructor: there is no sphere, the radius is zero and the position is
		// the zero vector.
		Sphere<T>();
		// Copy constructor.
		Sphere<T>(const Sphere<T>& aSphere);
		// Constructor that takes the center position and radius of the sphere.
		Sphere<T>(const Vector3<T>& aCenter, T aRadius);
		// Init the sphere with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius);
		Sphere<T> GetSphereinNewSpace(const Matrix4x4<T>& aMatrix) const;

		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInside(const Vector3<T>& aPosition) const;
		const Vector3<T> GetPoint() const;
		const T GetRadius() const;
		const T GetRadiusSqr() const;
	private:
		Vector3<T> myPoint;
		T myRadiusSqr;
		T myRadius;
	};

	template <class T>
	Sphere<T>::Sphere()
	{
		myPoint = Vector3<T>();
		myRadius = 0;
		myRadiusSqr = 0;
	}

	template <class T>
	Sphere<T>::Sphere(const Sphere<T>& aSphere)
	{
		myPoint = aSphere.myPoint;
		myRadius = aSphere.myRadius;
		myRadiusSqr = aSphere.myRadiusSqr;
	}

	template <class T>
	Sphere<T>::Sphere(const Vector3<T>& aCenter, T aRadius)
	{
		myPoint = aCenter;
		myRadius = aRadius;
		myRadiusSqr = aRadius * aRadius;
	}

	template <class T>
	void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
	{
		myPoint = aCenter;
		myRadius = aRadius;
		myRadiusSqr = aRadius * aRadius;
	}

	template<class T>
	inline Sphere<T> Sphere<T>::GetSphereinNewSpace(const Matrix4x4<T>& aMatrix) const
	{
		Vector3<T> newCenter = ToVector3(ToVector4(myPoint, 1.0f) * aMatrix);
		Vector3<T> scale = Vector3<T>::Abs(Matrix4x4<T>::CreateScaleVector(aMatrix));
		T maxScale = static_cast<T>(std::max(scale.x, std::max(scale.y, scale.z)));
		T newRadius = GetRadius() * maxScale;
		Sphere<T> newSphere(newCenter, newRadius);
		return newSphere;
	}

	template <class T>
	bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
	{
		Vector3<T> differenceFromPointToCenter = aPosition - myPoint;
		T distance = differenceFromPointToCenter.LengthSqr();

		if (distance <= myRadiusSqr)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	template <class T>
	const Vector3<T> Sphere<T>::GetPoint() const
	{
		return myPoint;
	}

	template<class T>
	inline const T Sphere<T>::GetRadius() const
	{
		return myRadius;
	}

	template <class T>
	const T Sphere<T>::GetRadiusSqr() const
	{
		return myRadiusSqr;
	}
}