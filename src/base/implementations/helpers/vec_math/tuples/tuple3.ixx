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
class Tuple3 {
public:
    // Constructors
    constexpr explicit Tuple3() noexcept : m_x{}, m_y{}, m_z{} {}
    constexpr explicit Tuple3(const T x, const T y, const T z) noexcept : m_x{ x }, m_y{ y }, m_z{ z }
    {
        HCheckNaNs(m_x, m_y, m_z);
    }
    constexpr Tuple3(const Tuple3&) = default;
    constexpr Tuple3(Tuple3&&) noexcept = default;
    constexpr Tuple3& operator=(const Tuple3&) = default;
    constexpr Tuple3& operator=(Tuple3&&) noexcept = default;

    constexpr auto operator<=>(const Tuple3&) const = default;

    constexpr Child<T> operator-() const
    {
        HCheckNaNs(m_x, m_y, m_z);
        return Child<T>(-m_x, -m_y, -m_z);
    }

    constexpr T& operator[](const std::size_t index) {
        HCheckIndex<T>(T(index), static_cast<int>(3));
        switch (index) {
        case 0: return m_x;
        case 1: return m_y;
        default: return m_z;
        }
    }

    constexpr const T& operator[](const std::size_t index) const {
        return const_cast<Tuple3&>(*this)[index];
    }

    // Arithmetic operators
    template <Arithmetic U>
    constexpr auto operator+(const Tuple3<Child, U>& other) const -> Child<decltype(T{} + U{})>
    {
        Child<decltype(T{} + U{}) > result{ this->m_x, this->m_y, this->m_z };
        result += other;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator-(const Tuple3<Child, U>& other) const-> Child<decltype(T{} - U{}) >
    {
        Child<decltype(T{} - U{}) > result{ this->m_x, this->m_y, this->m_z };
        result -= other;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator*(const U& scalar) const -> Child<decltype(T{} * U{})>
    {
        Child<decltype(T{} * U{}) > result{ this->m_x, this->m_y, this->m_z };
        result *= scalar;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator/(const U& scalar) const -> Child<decltype(T{} / U{})>
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

        m_x += other.m_x;
        m_y += other.m_y;
        m_z += other.m_z;

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator-=(const Tuple3<Child, U>& other)
    {
        HCheckNaNs(m_x, m_y, m_z, other.m_x, other.m_y, other.m_z);

        m_x -= other.m_x;
        m_y -= other.m_y;
        m_z -= other.m_z;

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator*=(const U& scalar)
    {
        HCheckNaNs(m_x, m_y, m_z, scalar);

        m_x *= scalar;
        m_y *= scalar;
        m_z *= scalar;

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