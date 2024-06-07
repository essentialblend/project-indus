export module tuple3;

import <concepts>;
import <cmath>;
import <stdexcept>;
import <immintrin.h>;
import <sstream>;
import <string>;

import h_miscutilities;
import h_mathutilities;

export template <template <typename> class Child, Arithmetic T>
class alignas(32) Tuple3 {
public:
    // Constructors
    constexpr explicit Tuple3() noexcept : m_x{}, m_y{}, m_z{} {}
    constexpr explicit Tuple3(const T x, const T y, const T z) noexcept : m_x{ x }, m_y{ y }, m_z{ z }
    {
        HCheckNaNs(x, y, z);
    }
    constexpr Tuple3(const Tuple3&) = default;
    constexpr Tuple3(Tuple3&&) noexcept = default;
    constexpr Tuple3& operator=(const Tuple3&) = default;
    constexpr Tuple3& operator=(Tuple3&&) noexcept = default;

    constexpr Child<T> operator-() const
    {
        HCheckNaNs(m_x, m_y, m_z);
        return Child<T>(-m_x, -m_y, -m_z);
    }

    constexpr T& operator[](const std::size_t index)
    {
        HCheckIndex<T>(T(index), static_cast<int>(3));

        switch (index) {
        case 0: return m_x;
        case 1: return m_y;
        default: return m_z;
        }
    }

    constexpr T operator[](const std::size_t index) const
    {
        return const_cast<T&>(std::as_const(*this)[index]);
    }

    // Arithmetic operators
    template <Arithmetic U>
    constexpr auto operator+(const Tuple3<Child, U>& other) const->Child<decltype(T{} + U{}) >
    {
        Child<decltype(T{} + U{}) > result{ this->m_x, this->m_y, this->m_z };
        result += other;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator-(const Tuple3<Child, U>& other) const->Child<decltype(T{} - U{}) >
    {
        Child<decltype(T{} - U{}) > result{ this->m_x, this->m_y, this->m_z };
        result -= other;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator*(const U& scalar) const->Child<decltype(T{} *U{}) >
    {
        Child<decltype(T{} *U{}) > result{ this->m_x, this->m_y, this->m_z };
        result *= scalar;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator/(const U& scalar) const->Child<decltype(T{} / U{}) >
    {
        Child<decltype(T{} / U{}) > result{ this->m_x, this->m_y, this->m_z };
        result /= scalar;
        return result;
    }

    // Compound assignment operators
    template <Arithmetic U>
    constexpr Child<T>& operator+=(const Tuple3<Child, U>& other)
    {
        HCheckNaNs(m_x, m_y, m_z, other.m_x, other.m_y, other.m_z);

        if constexpr (std::is_same_v<T, float>)
        {
            if (HIsAVX2Enabled())
            {
                __m128 this_vec = _mm_set_ps(0.0f, m_z, m_y, m_x);
                __m128 other_vec = _mm_set_ps(0.0f, other.m_z, other.m_y, other.m_x);
                __m128 result_vec = _mm_add_ps(this_vec, other_vec);
                _mm_store_ps(reinterpret_cast<float*>(&m_x), result_vec);
            }
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            if (HIsAVX2Enabled())
            {
                __m128d this_vec1 = _mm_set_pd(m_y, m_x);
                __m128d other_vec1 = _mm_set_pd(other.m_y, other.m_x);
                __m128d result_vec1 = _mm_add_pd(this_vec1, other_vec1);
                _mm_store_pd(&m_x, result_vec1);

                __m128d this_vec2 = _mm_set_sd(m_z);
                __m128d other_vec2 = _mm_set_sd(other.m_z);
                __m128d result_vec2 = _mm_add_sd(this_vec2, other_vec2);
                _mm_store_sd(&m_z, result_vec2);
            }
        }
        else
        {
            m_x += other.m_x;
            m_y += other.m_y;
            m_z += other.m_z;
        }

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator-=(const Tuple3<Child, U>& other)
    {
        HCheckNaNs(m_x, m_y, m_z, other.m_x, other.m_y, other.m_z);

        if constexpr (std::is_same_v<T, float>)
        {
            if (HIsAVX2Enabled())
            {
                __m128 this_vec = _mm_set_ps(0.0f, m_z, m_y, m_x);
                __m128 other_vec = _mm_set_ps(0.0f, other.m_z, other.m_y, other.m_x);
                __m128 result_vec = _mm_sub_ps(this_vec, other_vec);
                _mm_store_ps(reinterpret_cast<float*>(&m_x), result_vec);
            }
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            if (HIsAVX2Enabled())
            {
                __m128d this_vec1 = _mm_set_pd(m_y, m_x);
                __m128d other_vec1 = _mm_set_pd(other.m_y, other.m_x);
                __m128d result_vec1 = _mm_sub_pd(this_vec1, other_vec1);
                _mm_store_pd(&m_x, result_vec1);

                __m128d this_vec2 = _mm_set_sd(m_z);
                __m128d other_vec2 = _mm_set_sd(other.m_z);
                __m128d result_vec2 = _mm_sub_sd(this_vec2, other_vec2);
                _mm_store_sd(&m_z, result_vec2);
            }
        }
        else
        {
            m_x -= other.m_x;
            m_y -= other.m_y;
            m_z -= other.m_z;
        }

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator*=(const U& scalar)
    {
        HCheckNaNs(m_x, m_y, m_z, scalar);

        if constexpr (std::is_same_v<T, float>)
        {
            if (HIsAVX2Enabled())
            {
                __m128 this_vec = _mm_set_ps(0.0f, m_z, m_y, m_x);
                __m128 scalar_vec = _mm_set1_ps(scalar);
                __m128 result_vec = _mm_mul_ps(this_vec, scalar_vec);
                _mm_store_ps(reinterpret_cast<float*>(&m_x), result_vec);
            }
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            if (HIsAVX2Enabled())
            {
                __m128d this_vec1 = _mm_set_pd(m_y, m_x);
                __m128d scalar_vec1 = _mm_set1_pd(scalar);
                __m128d result_vec1 = _mm_mul_pd(this_vec1, scalar_vec1);
                _mm_store_pd(&m_x, result_vec1);

                __m128d this_vec2 = _mm_set_sd(m_z);
                __m128d scalar_vec2 = _mm_set1_pd(scalar);
                __m128d result_vec2 = _mm_mul_sd(this_vec2, scalar_vec2);
                _mm_store_sd(&m_z, result_vec2);
            }
        }
        else
        {
            m_x *= scalar;
            m_y *= scalar;
            m_z *= scalar;
        }

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator/=(const U& scalar)
    {
        HCheckNaNs(m_x, m_y, m_z, scalar);
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
                __m128 this_vec = _mm_set_ps(m_z, m_y, m_x, 0.0f);
                __m128 nan_vec = _mm_cmpunord_ps(this_vec, this_vec);
                int mask = _mm_movemask_ps(nan_vec);
                return mask != 0;
            }
        }
        if constexpr (std::is_same_v<T, double>)
        {
            if (HIsAVX2Enabled())
            {
                __m128d this_vec1 = _mm_set_pd(m_y, m_x);
                __m128d nan_vec1 = _mm_cmpunord_pd(this_vec1, this_vec1);
                int mask1 = _mm_movemask_pd(nan_vec1);

                __m128d this_vec2 = _mm_set_sd(m_z);
                __m128d nan_vec2 = _mm_cmpunord_pd(this_vec2, this_vec2);
                int mask2 = _mm_movemask_pd(nan_vec2);

                return mask1 != 0 || mask2 != 0;
            }
        }

        return std::isnan(m_x) || std::isnan(m_y) || std::isnan(m_z);
    }

    std::string toString() const
    {
        std::ostringstream oss{};
        oss << "Tuple3(" << m_x << ", " << m_y << ", " << m_z << ")";
        return oss.str();
    }

    static const unsigned int m_numDimensions{ 3 };

protected:
    T m_x{}, m_y{}, m_z{};
};