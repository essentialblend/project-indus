export module core_util;

// REDUNDANT.

import <type_traits>;
import <limits>;
import <numbers>;
import <random>;

export constexpr double UPi{ std::numbers::pi };
export constexpr double UInfinity{ std::numeric_limits<double>::infinity() };
export constexpr double UEpsilon{ std::numeric_limits<double>::epsilon() };

export constexpr double UDegreesToRadians(double degreesValue)
{
	return degreesValue * UPi / 180.0;
}

export constexpr double URadiansToDegrees(double radiansValue)
{
	return radiansValue * 180.0 / UPi;
}

export template<typename ValueType>
ValueType UGenRNG(ValueType min = 0, ValueType max = 1)
{
    static thread_local std::random_device rd;
    static thread_local std::mt19937 generator(rd());
    
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




