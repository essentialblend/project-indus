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
class Tuple2 {
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

    constexpr auto operator<=>(const Tuple2&) const = default;

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
        return const_cast<Tuple2&>(*this)[index];
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
        m_x += other.m_x;
        m_y += other.m_y;
        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator-=(const Tuple2<Child, U>& other)
    {
        HCheckNaNs(m_x, m_y, other.m_x, other.m_y);
        m_x -= other.m_x;
        m_y -= other.m_y;
        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator*=(const U& scalar)
    {
        HCheckNaNs(m_x, m_y, scalar);
        m_x *= scalar;
        m_y *= scalar;
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