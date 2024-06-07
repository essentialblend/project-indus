// This class template needs to be implemented in-place and cannot be separated into a header and source file because of partial in-class template specialization problems as of C++ 23.

export module tuple2;

import <concepts>;
import <cmath>;
import <stdexcept>;
import <immintrin.h>;
import <sstream>;
import <string>;

import h_miscutilities;
import h_mathutilities;

export template <template <typename> class Child, Arithmetic T>
class alignas(32) Tuple2 {
public:
    
    // Constructors
    constexpr explicit Tuple2() noexcept : m_x{}, m_y{} {}
    constexpr explicit Tuple2(const T x, const T y) noexcept : m_x{ x }, m_y{ y } 
    {
        HCheckNaNs(x, y);
    }
    constexpr Tuple2(const Tuple2&) = default;
    constexpr Tuple2(Tuple2&&) noexcept = default;
    constexpr Tuple2& operator=(const Tuple2&) = default;
    constexpr Tuple2& operator=(Tuple2&&) noexcept = default;

    constexpr Child<T> operator-() const
    {
        HCheckNaNs(m_x, m_y);
        return Child<T>(-m_x, -m_y);
    }

    constexpr T& operator[](const std::size_t index)
    {
        HCheckIndex<T>(T(index), static_cast<int>(2));

        return index == 0 ? m_x : m_y;
    }
    
    constexpr T operator[](const std::size_t index) const
    {
        return const_cast<T&>(std::as_const(*this)[index]);
    }

    // Arithmetic operators
    template <Arithmetic U>
    constexpr auto operator+(const Tuple2<Child, U>& other) const -> Child<decltype(T{} + U{})>
    {
        Child<decltype(T{} + U{})> result{ this->m_x, this->m_y };
		result += other;
		return result;
    }

    template <Arithmetic U>
    constexpr auto operator-(const Tuple2<Child, U>& other) const -> Child<decltype(T{} - U{})>
    {
        Child<decltype(T{} - U{}) > result{ this->m_x, this->m_y };
        result -= other;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator*(const U& scalar) const -> Child<decltype(T{} * U{})>
    {
        Child<decltype(T{} * U{})> result{ this->m_x, this->m_y };
		result *= scalar;
		return result;
    }

    template <Arithmetic U>
    constexpr auto operator/(const U& scalar) const -> Child<decltype(T{} / U{})>
    {
        Child<decltype(T{} / U{})> result{ this->m_x, this->m_y };
        result /= scalar;
        return result;
    }

    // Compound assignment operators
    template <Arithmetic U>
    constexpr Child<T>& operator+=(const Tuple2<Child, U>& other)
    {
        // Check for NaNs in debug build
        HCheckNaNs(m_x, m_y, other.m_x, other.m_y);

        if constexpr (std::is_same_v<T, float>)
        {
            if(HIsAVX2Enabled())
            {
                // Load the elements into 128-bit AVX registers
                __m128 this_vec = _mm_loadu_ps(&m_x);
                __m128 other_vec = _mm_loadu_ps(&other.m_x);
                __m128 result_vec = _mm_add_ps(this_vec, other_vec);
                _mm_storeu_ps(&m_x, result_vec);

            }
        }
        else if constexpr (std::is_same_v<T, double>) 
        {
            if (HIsAVX2Enabled())
            {
                __m128d this_vec = _mm_loadu_pd(&m_x);
                __m128d other_vec = _mm_loadu_pd(&other.m_x);
                __m128d result_vec = _mm_add_pd(this_vec, other_vec);
                _mm_storeu_pd(&m_x, result_vec);
            }
        }
        else 
        {
            m_x += other.m_x;
            m_y += other.m_y;
        }

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator-=(const Tuple2<Child, U>& other)
    {
        HCheckNaNs(m_x, m_y, other.m_x, other.m_y);

        if constexpr (std::is_same_v<T, float>) 
        {
            if (HIsAVX2Enabled())
            {
                __m128 this_vec = _mm_loadu_ps(&m_x);
                __m128 other_vec = _mm_loadu_ps(&other.m_x);
                __m128 result_vec = _mm_sub_ps(this_vec, other_vec);
                _mm_storeu_ps(&m_x, result_vec);
            }
        }
        else if constexpr (std::is_same_v<T, double>) 
        {
            if (HIsAVX2Enabled())
            {
                __m128d this_vec = _mm_loadu_pd(&m_x);
                __m128d other_vec = _mm_loadu_pd(&other.m_x);
                __m128d result_vec = _mm_sub_pd(this_vec, other_vec);
                _mm_storeu_pd(&m_x, result_vec);
            }
        }
        else 
        {
            m_x -= other.m_x;
            m_y -= other.m_y;
        }

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator*=(const U& scalar)
    {
        HCheckNaNs(m_x, m_y, scalar);

        if constexpr (std::is_same_v<T, float>) 
        {
            if (HIsAVX2Enabled())
            {
                __m128 this_vec = _mm_loadu_ps(&m_x);
                __m128 scalar_vec = _mm_set1_ps(scalar);
                __m128 result_vec = _mm_mul_ps(this_vec, scalar_vec);
                _mm_storeu_ps(&m_x, result_vec);
            }
        }
        else if constexpr (std::is_same_v<T, double>) 
        {
            if (HIsAVX2Enabled())
            {
                __m128d this_vec = _mm_loadu_pd(&m_x);
                __m128d scalar_vec = _mm_set1_pd(scalar);
                __m128d result_vec = _mm_mul_pd(this_vec, scalar_vec);
                _mm_storeu_pd(&m_x, result_vec);
            }
        }
        else 
        {
            m_x *= scalar;
            m_y *= scalar;
        }

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator/=(const U& scalar)
    {
        HCheckNaNs(m_x, m_y, scalar);
        HCheckDivByZero(scalar);

        T reciprocal{ T{ 1 } / scalar };

        return (*this) *= reciprocal;
    }

    bool hasNaN() const
    {
		if constexpr (std::is_same_v<T, float>)
        {
            if (HIsAVX2Enabled())
            {
                __m128 this_vec = _mm_set_ps(m_y, m_x, 0.0f, 0.0f);
                __m128 nan_vec = _mm_cmpunord_ps(this_vec, this_vec);
                int mask = _mm_movemask_ps(nan_vec);
                return mask != 0;
            }
		}
		if constexpr (std::is_same_v<T, double>)
		{
            if (HIsAVX2Enabled())
            {
                __m128d this_vec = _mm_set_pd(m_y, m_x);
                __m128d nan_vec = _mm_cmpunord_pd(this_vec, this_vec);
                int mask = _mm_movemask_pd(nan_vec);
                return mask != 0;
            }
		}
        
        return std::isnan(m_x) || std::isnan(m_y);
    }
    
    std::string toString() const
    {
        std::ostringstream oss{};
        oss << "Tuple2(" << m_x << ", " << m_y << ")";
        return oss.str();
    }

    ~Tuple2() noexcept = default;

    static const unsigned int m_numDimensions{ 2 };
protected:
    T m_x{}, m_y{};
};