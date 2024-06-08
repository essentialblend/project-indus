export module h_mathutilities;

import <limits>;
import <numbers>;
import <cmath>;
import <string>;
import <algorithm>;
import <stdexcept>;
import <random>;
import <print>;
import <immintrin.h>;

import h_miscutilities;

export
{
    // Types
    using Float32 = float;
    using Float64 = double;
    using Float = Float64;

    // Constants.
    constexpr Float HPi{ std::numbers::pi };
    constexpr Float HInfinity{ std::numeric_limits<Float>::infinity() };
    constexpr Float HEpsilon{ std::numeric_limits<Float>::epsilon() };

    // Concepts
    template <typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template <typename T>
    concept Container = requires(T t) {
        typename T::value_type;
        { T::m_numDimensions } -> std::convertible_to<std::size_t>;
        { t[0] } -> std::convertible_to<typename T::value_type>;
            requires Arithmetic<typename T::value_type>;
        { t.begin() } -> std::input_iterator;
        { t.end() } -> std::input_iterator;
    };

    template<typename T>
    concept HasNaNCheck = requires(T t)
    {
        { t.hasNaN() } -> std::same_as<bool>;
    };

    // Debug checks.
    template <typename... Ts>
    void HCheckNaNs([[maybe_unused]] Ts... values) {
        if constexpr (H_IS_DEBUG_BUILD)
        {
            if ((... || std::isnan(values))) 
            {
                throw std::runtime_error("NaN detected");
            }
        }
    }

    template <HasNaNCheck T>
    void HCheckNaNs(const T& obj) 
    {
        if constexpr (H_IS_DEBUG_BUILD) 
        {
            if (obj.hasNaN()) 
            {
                throw std::runtime_error("NaN detected");
            }
        }
    }

    template<typename T>
    void HCheckIndex(const T index, const T maxIndex) 
    {
        if constexpr (H_IS_DEBUG_BUILD)
        {
            if (index < 0 || index >= maxIndex) 
            {
                throw std::out_of_range("Index " + std::to_string(index) + " out of bounds [0, " + std::to_string(maxIndex - 1) + "]");
            }
        }
    }

    template <typename T>
    void HCheckDivByZero(const T scalar) 
    {
        if constexpr (H_IS_DEBUG_BUILD) 
        {
            if (scalar == 0) 
            {
                throw std::runtime_error("Division by zero");
            }
        }
    }

    // Misc. math/RNG.
    constexpr Float HDegreesToRadians(const Float degreesValue)
    {
        return degreesValue * HPi / 180.0;
    }

    constexpr Float HRadiansToDegrees(const Float radiansValue)
    {
        return radiansValue * 180.0 / HPi;
    }

    template <typename ValueType>
    constexpr ValueType HGenRNG(const ValueType min = 0, const ValueType max = 1)
    {
        static thread_local std::random_device rd;
        static thread_local std::mt19937 generator(rd());

        if constexpr (std::is_integral_v<ValueType>)
        {
            std::uniform_int_distribution<ValueType> distr(min, max);
            return distr(generator);
        }
        std::uniform_real_distribution<ValueType> distr(min, max);
        return distr(generator);
    }

    // For fundamental types (T).
    template <typename T>
    constexpr T HFusedMultiplyAdd(const T a, const T b, const T c) noexcept {
        if constexpr (std::is_integral_v<T>) {
            return a * b + c;
        }
        if constexpr (std::is_floating_point_v<T>) {
            return std::fma(a, b, c);
        }
    }

    template <typename T>
    constexpr T HDifferenceOfProducts(const T a, const T b, const T c, const T d) noexcept 
    {
        auto cd{ c * d };
        auto diffOfProducts{ HFusedMultiplyAdd<T>(a, b, -cd) };
        auto error{ HFusedMultiplyAdd(-c, d, cd) };

        return diffOfProducts + error;
    }

    template <typename T>
    constexpr T HSumOfProducts(const T a, const T b, const T c, const T d) noexcept
    {
        auto cd{ c * d };
        auto sumOfProducts{ HFusedMultiplyAdd<T>(a, b, cd) };
        auto error{ HFusedMultiplyAdd(c, d, -cd) };

        return sumOfProducts + error;
    }
    // CompensatedFloat struct and non-members.
    template <typename Float>
    struct CompensatedFloat
    {
        Float m_value{};
        Float m_precisionErrors{};

        constexpr explicit CompensatedFloat(Float v = 0, Float err = 0) : m_value(v), m_precisionErrors(err) {}

        operator Float() const { return m_value + m_precisionErrors; }
    };

    // TwoSum function: Computes the sum of two floating-point numbers and returns the result with compensation for numerical error.
    template <typename Float>
    constexpr CompensatedFloat<Float> computeCompensatedTwoSum(const Float a, const Float b)
    {
        Float sum = a + b;
        Float delta = sum - a;

        return CompensatedFloat<Float>{ sum, (a - (sum - delta)) + (b - delta) };
    }

    // TwoProd function: Computes the product of two floating-point numbers and returns the result with compensation for numerical error.
    template <typename Float>
    constexpr CompensatedFloat<Float> computeCompensatedTwoProduct(const Float a, const Float b)
    {
        Float prod = a * b;
        Float err = HFusedMultiplyAdd(a, b, -prod);
        return CompensatedFloat<Float>{ prod, err };
    }

    template <typename T, typename... Ts>
    constexpr T HInnerProduct(T a, T b, Ts... terms) noexcept 
    {
        auto computeCompensatedInnerProduct = [](auto&& self, T x, T y, auto... rest) -> CompensatedFloat<T> 
        {
				if constexpr (sizeof...(rest) == 0)
				{
					return computeCompensatedTwoProduct(x, y);
				}
				else
				{
					auto ab = computeCompensatedTwoProduct(x, y);
					auto tp = self(self, rest...);
					auto sum = computeCompensatedTwoSum(ab.m_value, tp.m_value);
					return CompensatedFloat<T>{sum.m_value, ab.m_precisionErrors + tp.m_precisionErrors + sum.m_precisionErrors};
				}
        };

        auto result = computeCompensatedInnerProduct(computeCompensatedInnerProduct, a, b, terms...);
        return static_cast<T>(result);
    }

    // Fully generic math utilities.
    template<Container ContainerClass, Arithmetic ScalarType>
    constexpr decltype(auto) operator*(const ScalarType s, const ContainerClass& t)
	{
        HCheckNaNs(t);
		return t * s;
	}

    template<Container ContainerClass>
    constexpr auto HComputeAbs(const ContainerClass& t)
    {
        ContainerClass result{};

        for (std::size_t i = 0; i < ContainerClass::m_numDimensions; ++i)
        {
            result[i] = std::abs(t[i]);
        }
        return result;
    }

    template<Container ContainerClass>
    constexpr decltype(auto) HComputeCeil(const ContainerClass& t)
    {
        ContainerClass result{};
        for (std::size_t i{}; i < ContainerClass::m_numDimensions; ++i)
        {
            result[i] = std::ceil(t[i]);
        }

        return result;
    }

    template<Container ContainerClass>
    constexpr decltype(auto) HComputeFloor(const ContainerClass& t)
    {
        ContainerClass result{};
        for (std::size_t i{}; i < ContainerClass::m_numDimensions; ++i)
        {
            result[i] = std::floor(t[i]);
        }
        return result;
    }


    template<Container ContainerClass, Arithmetic ElementType>
    constexpr decltype(auto) HLerp(const ElementType t, const ContainerClass& v1, const ContainerClass& v2)
	{
		return (1 - t) * v1 + t * v2;
	}

    template<Container ContainerClass>
    constexpr ContainerClass HFusedMultiplyAdd(const Float a, const ContainerClass& b, const ContainerClass& c)
    {
        ContainerClass result{};
        for (std::size_t i{}; i < ContainerClass::m_numDimensions; ++i)
        {
            result[i] = HFusedMultiplyAdd(a, b[i], c[i]);
        }
        return result;
    }

    template<Container ContainerClass>
    constexpr ContainerClass HFusedMultiplyAdd(const ContainerClass& a, const Float b, const ContainerClass& c)
    {
        return HFusedMultiplyAdd(b, a, c);
    }

    template<Container ContainerClass>
    constexpr decltype(auto) HGetMin(const ContainerClass& t1, const ContainerClass& t2)
    {
        ContainerClass result{};
        for (std::size_t i{}; i < ContainerClass::m_numDimensions; ++i)
        {
            result[i] = std::min(t1[i], t2[i]);
        }
        return result;
    }

    template<Container ContainerClass>
    constexpr decltype(auto) HGetMax(const ContainerClass& t1, const ContainerClass& t2)
    {
        
        ContainerClass result{};
        for (std::size_t i{}; i < ContainerClass::m_numDimensions; ++i)
        {
            result[i] = std::max(t1[i], t2[i]);
        }
        return result;
    }

    template<Container ContainerClass, std::size_t... Indices>
    constexpr ContainerClass HPermute(const ContainerClass& t) {
        ContainerClass result{};

        ((result[Indices] = t[Indices]), ...);

        return result;
    }

    template<Container ContainerClass>
    constexpr auto HProd(const ContainerClass& t)
    {
        using ElementType = typename ContainerClass::value_type;
        ElementType result{ 1 };
        for (std::size_t i{}; i < ContainerClass::m_numDimensions; ++i)
        {
            result *= t[i];
        }
        return result;
    }

    template<Container ContainerClass, Arithmetic ElementType>
    constexpr ElementType HGetMinComponentValue(const ContainerClass& t)
	{
        return *std::min_element(t.begin(), t.end());
	}

    template<Container ContainerClass, Arithmetic ElementType>
    constexpr ElementType HGetMaxComponentValue(const ContainerClass& t)
    {
        return *std::max_element(t.begin(), t.end());
    }

    template<Container ContainerClass>
    constexpr std::size_t HGetMinComponentIndex(const ContainerClass& t)
    {
        return static_cast<std::size_t>(std::distance(t.begin(), std::min_element(t.begin(), t.end())));
    }

    template<Container ContainerClass>
    constexpr std::size_t HGetMaxComponentIndex(const ContainerClass& t)
    {
        return static_cast<std::size_t>(std::distance(t.begin(), std::max_element(t.begin(), t.end())));
    }

    template<Container ContainerClass>
    constexpr auto HComputeDot(const ContainerClass& a, const ContainerClass& b)
    {
        using ElementType = typename ContainerClass::value_type;
        ElementType result{};

        for (std::size_t i = 0; i < ContainerClass::m_numDimensions; ++i)
        {
            result += a[i] * b[i];
        }

        return result;
    }

    template<Container ContainerClass>
    constexpr auto HComputeLengthSq(const ContainerClass& t) 
    {
        return HComputeDot<ContainerClass>(t, t);
    }

    template<Container ContainerClass>
    constexpr Float HComputeLength(const ContainerClass& t) 
    {
        return static_cast<Float>(std::sqrt(HComputeLengthSq<ContainerClass>(t)));
    }

    template<Container ContainerClass>
    constexpr auto HComputeDistanceSq(const ContainerClass& t1, const ContainerClass& t2)
    {
        using ElementType = typename ContainerClass::value_type;
        ElementType result{};

        for (std::size_t i = 0; i < ContainerClass::m_numDimensions; ++i)
        {
            ElementType diff = t1[i] - t2[i];
            result += diff * diff;
        }
        return result;
    }


    template<Container ContainerClass>
    constexpr Float HComputeDistance(const ContainerClass& t1, const ContainerClass& t2)
    {
        return static_cast<Float>(std::sqrt(HComputeDistanceSq<ContainerClass>(t1, t2)));
    }
};

