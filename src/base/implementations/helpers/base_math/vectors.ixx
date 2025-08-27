export module vectors;

import <string>;
import <cmath>;

import h_mathutilities;

export template<Arithmetic U>
class Point2;

export template <template <typename> class Child, Arithmetic T>
class Tuple2 {
public:

    // Constructors
    constexpr explicit Tuple2() noexcept : m_x{}, m_y{} {}
    constexpr explicit Tuple2(T x, T y) noexcept : m_x{ x }, m_y{ y }
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

    constexpr T& operator[](std::size_t index)
    {
        HCheckIndex<T>(T(index), static_cast<int>(2));

        return index == 0 ? m_x : m_y;
    }

    constexpr T operator[](std::size_t index) const
    {
        return const_cast<Tuple2&>(*this)[index];
    }

    // Arithmetic operators
    template <Arithmetic U>
    constexpr auto operator+(const Tuple2<Child, U>& other) const->Child<decltype(T{} + U{}) >
    {
        Child<decltype(T{} + U{}) > result{ this->m_x, this->m_y };
        result += other;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator-(const Tuple2<Child, U>& other) const->Child<decltype(T{} - U{}) >
    {
        Child<decltype(T{} - U{}) > result{ this->m_x, this->m_y };
        result -= other;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator*(U scalar) const->Child<decltype(T{} *U{}) >
    {
        Child<decltype(T{} *U{}) > result{ this->m_x, this->m_y };
        result *= scalar;
        return result;
    }

    template <Arithmetic U>
    constexpr auto operator/(U scalar) const->Child<decltype(T{} / U{}) >
    {
        Child<decltype(T{} / U{}) > result{ this->m_x, this->m_y };
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
    constexpr Child<T>& operator*=(U scalar)
    {
        HCheckNaNs(m_x, m_y, scalar);
        m_x *= scalar;
        m_y *= scalar;
        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator/=(U scalar)
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

export template <template <typename> class Child, Arithmetic T>
class Tuple3 {
public:
    // Constructors
    constexpr explicit Tuple3() noexcept : m_x{}, m_y{}, m_z{} {}
    constexpr explicit Tuple3(T x, T y, T z) noexcept : m_x{ x }, m_y{ y }, m_z{ z }
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

    constexpr T& operator[](std::size_t index) {
        HCheckIndex<T>(T(index), static_cast<int>(3));
        switch (index) {
        case 0: return m_x;
        case 1: return m_y;
        default: return m_z;
        }
    }

    constexpr const T& operator[](std::size_t index) const {
        return const_cast<Tuple3&>(*this)[index];
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
    constexpr Child<T>& operator*=(U scalar)
    {
        HCheckNaNs(m_x, m_y, m_z, scalar);

        m_x *= scalar;
        m_y *= scalar;
        m_z *= scalar;

        return static_cast<Child<T>&>(*this);
    }

    template <Arithmetic U>
    constexpr Child<T>& operator/=(U scalar)
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

export template <Arithmetic T>
class Vector2 : public Tuple2<Vector2, T> {
public:
    using Tuple2<Vector2, T>::m_x;
    using Tuple2<Vector2, T>::m_y;

    template <Arithmetic U>
    explicit Vector2(Point2<U> p) : Tuple2<Vector2, T>(T(p.m_x), T(p.m_y)) {}

    constexpr Vector2() noexcept : Tuple2<Vector2, T>() {}

    template <Arithmetic U>
    constexpr explicit Vector2(const Vector2<U>& v) noexcept : Tuple2<Vector2, T>(T(v.m_x), T(v.m_y)) {}
    template <Arithmetic U>
    constexpr explicit Vector2(const Point2<U>& p) noexcept : Tuple2<Vector2, T>(T(p.m_x), T(p.m_y)) {}
    
    constexpr explicit Vector2(T x, T y) noexcept : Tuple2<Vector2, T>(x, y) {}

};

export template <Arithmetic T>
class Vector3 : public Tuple3<Vector3, T> {
public:
    using Tuple3<Vector3, T>::m_x;
    using Tuple3<Vector3, T>::m_y;
    using Tuple3<Vector3, T>::m_z;

    constexpr explicit Vector3() noexcept : Tuple3<Vector3, T>() {}

    template <Arithmetic U>
    constexpr explicit Vector3(Vector3<U> v) noexcept : Tuple3<Vector3, T>(T(v.m_x), T(v.m_y), T(v.m_z)) {}

    constexpr explicit Vector3(T x, T y, T z) noexcept : Tuple3<Vector3, T>(x, y, z) {}

    // Point3 and Normal3 conversion.
};

export
{
    using Vec2f = Vector2<Float32>;
    using Vec2d = Vector2<Float64>;
    using Vec2i = Vector2<int>;

    // Vec2c: Current precision: double / Float64 / Float.
    using Vec2c = Vec2d;


    using Vec3f = Vector3<Float32>;
    using Vec3d = Vector3<Float64>;
    using Vec3i = Vector3<int>;

    // Vec2c: Current precision: double / Float64 / Float.
    using Vec3c = Vec3d;
}
