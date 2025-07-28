#include <random>
#include <type_traits>
#include <Math/ArithmeticUtilities.hpp>

namespace Utilities
{
	inline thread_local std::mt19937_64 ranGen(std::random_device{}());

	// Range is inclusive
	template<Math::IsIntegralType T>
	inline T RandomInRange(T aMin, T aMax)
	{
		std::uniform_int_distribution<T> distribution(aMin, aMax);
		return distribution(ranGen);
	}

	// Range is inclusive
	template<Math::IsFloatingPointType T>
	inline T RandomInRange(T aMin, T aMax)
	{
		std::uniform_real_distribution<T> distribution(aMin, aMax);
		return distribution(ranGen);
	}

	// Range is inclusive
	template<Math::IsIntegralType T>
	inline T RandomVariation(T aValue, T aVariation)
	{
		std::uniform_int_distribution<T> distribution(aValue - aVariation, aValue + aVariation);
		return distribution(ranGen);
	}

	// Range is inclusive
	template<Math::IsFloatingPointType T>
	inline T RandomVariation(T aValue, T aVariation)
	{
		std::uniform_real_distribution<T> distribution(aValue - aVariation, aValue + aVariation);
		return distribution(ranGen);
	}
}