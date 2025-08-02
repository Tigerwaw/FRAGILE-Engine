#pragma once
#include <math.h>
#include <limits>
#include <concepts>

namespace Math
{
	template<typename T = float>
	T EPSILON_V = std::numeric_limits<T>::epsilon();

	template<typename T>
	concept IsArithmeticType = std::is_arithmetic_v<T>;

	template<typename T>
	concept IsIntegralType = std::is_integral_v<T>;

	template<typename T>
	concept IsFloatingPointType = std::is_floating_point_v<T>;

	template<typename T>
	T Saturate(const T& aValue)
	{
		return static_cast<T>(std::fmin(std::fmax(aValue, T(0)), T(1)));
	}

	template<IsArithmeticType T>
	auto Equal(T aFirst, T aSecond, T aTolerance = EPSILON_V<T>)
	{
		return std::abs(aFirst - aSecond) <= aTolerance;
	}

	template<IsArithmeticType T>
	T Sign(T aValue)
	{
		return static_cast<T>((aValue < T{}) ? -1 : 1);
	}

	template<IsArithmeticType T>
	T IsInRange(T aValue, T aMinValue, T aMaxValue)
	{
		return aValue > aMinValue && aValue < aMaxValue;
	}
}