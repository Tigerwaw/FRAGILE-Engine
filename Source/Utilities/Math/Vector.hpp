#pragma once
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace Math
{
	template<class T>
	static Vector2<T> ToVector2(Vector3<T> aVector3)
	{
		return Vector2<T>(aVector3.x, aVector3.y);
	}

	template<class T>
	static Vector2<T> ToVector2(Vector4<T> aVector4)
	{
		return Vector2<T>(aVector4.x, aVector4.y);
	}

	template<class T>
	static Vector3<T> ToVector3(Vector2<T> aVector2, T aZ)
	{
		return Vector3<T>(aVector2.x, aVector2.y, aZ);
	}

	template<class T>
	static Vector3<T> ToVector3(Vector4<T> aVector4)
	{
		return Vector3<T>(aVector4.x, aVector4.y, aVector4.z);
	}

	template<class T>
	static Vector4<T> ToVector4(Vector2<T> aVector2, T aZ, T aW)
	{
		return Vector4<T>(aVector2.x, aVector2.y, aZ, aW);
	}

	template<class T>
	static Vector4<T> ToVector4(Vector3<T> aVector3, T aW )
	{
		return Vector4<T>(aVector3.x, aVector3.y, aVector3.z, aW);
	}
}