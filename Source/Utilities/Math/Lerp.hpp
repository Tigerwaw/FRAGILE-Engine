#pragma once
#include "Math/Vector.hpp"


namespace Math
{
	template<typename T>
	Math::Vector2<T> Lerp(Math::Vector2<T> aStartValue, Math::Vector2<T> aEndValue, const float aDeltaValue)
	{
		return Math::Vector2<T>::Lerp(aStartValue, aEndValue, aDeltaValue);
	}

	template<typename T>
	Math::Vector3<T> Lerp(Math::Vector3<T> aStartValue, Math::Vector3<T> aEndValue, const float aDeltaValue)
	{
		return Math::Vector3<T>::Lerp(aStartValue, aEndValue, aDeltaValue);
	}

	template<typename T>
	Math::Vector4<T> Lerp(Math::Vector4<T> aStartValue, Math::Vector4<T> aEndValue, const float aDeltaValue)
	{
		return Math::Vector4<T>::Lerp(aStartValue, aEndValue, aDeltaValue);
	}

	template<typename T>
	T Lerp(T aStartValue, T aEndValue, const float aDeltaValue)
	{
		return (aStartValue + aDeltaValue * (aEndValue - aStartValue));
	}

	template<typename T>
	T LerpAngle(T aStartValue, T aEndValue, const float aDeltaValue)
	{
		float dTheta = aEndValue - aStartValue;
		if (dTheta > 180.0f)
			aStartValue += 360.0f;
		else if (dTheta < -180.0f)
			aStartValue -= 360.0f;

		return Lerp(aStartValue, aEndValue, aDeltaValue);
	}
}