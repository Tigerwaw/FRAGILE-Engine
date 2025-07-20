#pragma once
#include <cmath>
#include <cassert>
#include <tuple>
#include "ArithmeticUtilities.hpp"

namespace Math
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		Vector3<T>();
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const Vector3<T>& aVector) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
		~Vector3<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector3<T> from this CommonUtillities::Vector3<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		Vector3<T> operator-() const;
		T LengthSqr() const;
		T Length() const;
		Vector3<T> GetNormalized() const;

		void Normalize();
		T Dot(const Vector3<T>& aVector) const;
		Vector3<T> Cross(const Vector3<T>& aVector) const;
		static Vector3<T> Abs(const Vector3<T>& aVector);
		static T Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
		static T DistanceSqr(const Vector3& aCurrent, const Vector3& aTarget);
		static T DistanceSqrToLine(const Vector3& aStart, const Vector3& aEnd, const Vector3& aPoint);
		static Vector3<T> Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const T aPercent);
		static Vector3 ClosestPointOnLine(const Vector3& aStart, const Vector3& aEnd, const Vector3& aPoint);
		static Vector3 ClosestPointOnSegment(const Vector3& aStart, const Vector3& aEnd, const Vector3& aPoint);
		static std::tuple<Vector3, Vector3> ClosestPointsSegmentSegment(const Vector3& aFirstStart, const Vector3& aFirstEnd, const Vector3& aSecondStart, const Vector3& aSecondEnd);
		static bool Equal(const Vector3& aLeft, const Vector3& aRight, T aTolerance = EPSILON_V<T>);
	};

	template<class T>
	Vector3<T>::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	template<class T>
	Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}

	template<class T>
	template<class OtherVectorClass>
	inline Vector3<T>::operator OtherVectorClass() const 
	{
		return { x, y, z };
	}

	template<class T>
	Vector3<T> Vector3<T>::operator-() const
	{
		return Vector3<T>(-x, -y, -z);
	}

	template<class T>
	T Vector3<T>::LengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	template<class T>
	T Vector3<T>::Length() const
	{
		return static_cast<T>(sqrt(LengthSqr()));
	}

	template<class T>
	Vector3<T> Vector3<T>::GetNormalized() const
	{
		T length = Length();
		length = max(length, (T)0.0001);
		Vector3<T> newVector(x / length, y / length, z / length);
		return newVector;
	}

	template<class T>
	void Vector3<T>::Normalize()
	{
		*this = GetNormalized();
	}


	template<class T>
	T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z;
	}

	template<class T>
	Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		T newX = y * aVector.z - z * aVector.y;
		T newY = z * aVector.x - x * aVector.z;
		T newZ = x * aVector.y - y * aVector.x;
		return Vector3<T>(newX, newY, newZ);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Abs(const Vector3<T>& aVector)
	{
		return { static_cast<T>(abs(aVector.x)), static_cast<T>(abs(aVector.y)), static_cast<T>(abs(aVector.z)) };
	}

	template<class T>
	inline T Vector3<T>::Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		const Vector3<T> direction = aVector1 - aVector0;
		return direction.Length();
	}

	template<typename T>
	T Vector3<T>::DistanceSqr(const Vector3& aCurrent, const Vector3& aTarget)
	{
		return (aTarget - aCurrent).LengthSqr();
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const T aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}

	template<typename T>
	Vector3<T> Vector3<T>::ClosestPointOnLine(const Vector3& aStart, const Vector3& aEnd, const Vector3& aPoint)
	{
		const Vector3<T> dir = aEnd - aStart;
		const T dirLengthSqr = dir.LengthSqr();

		if (dirLengthSqr <= EPSILON_V<T>)
			return aStart;

		const T t = dir.Dot(aPoint - aStart) / dirLengthSqr;

		return aStart + t * dir;
	}

	template<typename T>
	Vector3<T> Vector3<T>::ClosestPointOnSegment(const Vector3& aStart, const Vector3& aEnd, const Vector3& aPoint)
	{
		const Vector3<T> dir = aEnd - aStart;
		const T dirLengthSqr = dir.LengthSqr();

		if (dirLengthSqr <= EPSILON_V<T>)
			return aStart;

		const T t = dir.Dot(aPoint - aStart) / dirLengthSqr;

		return aStart + Saturate(t) * dir;
	}

	template<class T>
	inline std::tuple<Vector3<T>, Vector3<T>> Vector3<T>::ClosestPointsSegmentSegment(const Vector3& aFirstStart, const Vector3& aFirstEnd, const Vector3& aSecondStart, const Vector3& aSecondEnd)
	{
		const Vector3<T> AB = aFirstEnd - aFirstStart;
		const Vector3<T> CD = aSecondEnd - aSecondStart;
		const Vector3<T> CA = aFirstStart - aSecondStart;

		const T ABdotAB = AB.Dot(AB);
		const T CDdotCD = CD.Dot(CD); 
		const T CDdotCA = CD.Dot(CA); 

		T s{};
		T t{};

		if (ABdotAB <= EPSILON_V<T> && CDdotCD <= EPSILON_V<T>)
		{
			s = t = T(0);
		}
		else if (ABdotAB <= EPSILON_V<T>)
		{
			s = T(0);
			t = Saturate(CDdotCA / CDdotCD);
		}
		else
		{
			const T ABdotCA = AB.Dot(CA);
			if (CDdotCD <= EPSILON_V<T>)
			{
				s = Saturate(-ABdotCA / ABdotAB);
				t = T(0);
			}
			else
			{
				const T ABdotCD = AB.Dot(CD);
				const T denom = ABdotAB * CDdotCD - ABdotCD * ABdotCD;

				s = (denom != T(0)) ? Saturate((ABdotCD * CDdotCA - ABdotCA * CDdotCD) / denom) : T(0);

				t = (ABdotCD * s + CDdotCA) / CDdotCD;

				if (t < T(0))
				{
					s = Saturate(-ABdotCA / ABdotAB);
					t = T(0);
				}
				else if (t > T(1))
				{
					s = Saturate((ABdotCD - ABdotCA) / ABdotAB);
					t = T(1);
				}
			}
		}

		const Vector3<T> p1 = aFirstStart + AB * s;
		const Vector3<T> p2 = aSecondStart + CD * t;

		return std::make_tuple(p1, p2);
	}

	template<typename T>
	bool Vector3<T>::Equal(const Vector3& aLeft, const Vector3& aRight, T aTolerance)
	{
		return Math::Equal(aLeft.x, aRight.x, aTolerance) &&
			Math::Equal(aLeft.y, aRight.y, aTolerance) &&
			Math::Equal(aLeft.z, aRight.z, aTolerance);
	}

	template<typename T>
	T Vector3<T>::DistanceSqrToLine(const Vector3& aStart, const Vector3& aEnd, const Vector3& aPoint)
	{
		return Vector3<T>::DistanceSqr(ClosestPointOnLine(aStart, aEnd, aPoint), aPoint);
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		T newX = aVector0.x + aVector1.x;
		T newY = aVector0.y + aVector1.y;
		T newZ = aVector0.z + aVector1.z;
		return Vector3<T>(newX, newY, newZ);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		T newX = aVector0.x - aVector1.x;
		T newY = aVector0.y - aVector1.y;
		T newZ = aVector0.z - aVector1.z;
		return Vector3<T>(newX, newY, newZ);
	}

	// Returns the vector aVector1 component - multiplied by aVector1
	template <class T>
	Vector3<T> operator*(const Vector3<T>&aVector0, const Vector3<T>&aVector1)
	{
		return Vector3<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar) 
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return aVector * aScalar;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar) 
	{
		return aVector * (1 / aScalar);
	}

	//Component-wise division
	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const Vector3<T>& aOtherVector)
	{
		Vector3<T> vector;
		vector.x = aVector.x / aOtherVector.x;
		vector.y = aVector.y / aOtherVector.y;
		vector.z = aVector.z / aOtherVector.z;
		return vector;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar) 
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	bool operator==(const Vector3<T>& aVector, const Vector3<T>& aVector1)
	{
		if (aVector.x != aVector1.x) { return false; }
		if (aVector.y != aVector1.y) { return false; }
		if (aVector.z != aVector1.z) { return false; }

		return true;
	}


	typedef Vector3<float> Vector3f;
}