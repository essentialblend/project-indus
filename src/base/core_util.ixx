export module core_util;

import <type_traits>;
import <limits>;
import <numbers>;
import <random>;

export const double UPi{ std::numbers::pi };
export const double UInfinity{ std::numeric_limits<double>::infinity() };

export double UDegreesToRadians(double degreesValue)
{
	return degreesValue * UPi / 180.0;
}

export double URadiansToDegrees(double radiansValue)
{
	return radiansValue * 180.0 / UPi;
}

export template<typename ValueType>
ValueType UGenRNG(ValueType min = 0, ValueType max = 1)
{
	static thread_local std::mt19937 generator{ std::random_device(min, max) };

	if constexpr (std::is_integral_v<ValueType>)
	{
		std::uniform_int_distribution<ValueType> distr(min, max);
		return distr(generator);
	}
	else
	{
		std::uniform_real_distribution<ValueType> distr(min, max);
		return distr(generator);
	}

}





