export module points;

import h_mathutilities;
import vectors;

export template<Arithmetic T>
class Point2 : public Tuple2<Point2, T> 
{
public:
    using Tuple2<Point2, T>::m_x;
    using Tuple2<Point2, T>::m_y;
    using Tuple2<Point2, T>::operator+;
    using Tuple2<Point2, T>::operator+=;
    using Tuple2<Point2, T>::operator*;
    using Tuple2<Point2, T>::operator*=;

    constexpr explicit Point2() noexcept : Tuple2<Point2, T>() {}
    constexpr Point2(T x, T y) noexcept : Tuple2<Point2, T>(x, y) {}

    template<Arithmetic U>
    constexpr explicit Point2(const Vector2<U>& v) noexcept : Tuple2<Point2, T>(T(v.m_x), T(v.m_y)) {}

    constexpr Point2<T> operator-() const { return { -m_x, -m_y }; }

    template<Arithmetic U>
    constexpr Point2<T>& operator+=(const Vector2<U>& v)
    {
        HCheckNaNs(v);
        m_x += v.m_x;
        m_y += v.m_y;
        return *this;
    }

    template<Arithmetic U>
    constexpr auto operator+(const Vector2<U>& v) const->Point2<decltype(T{} + U{})>
    {
        Point2<decltype(T{} + U{}) > result = *this;
        result += v;
        return result;
    }

    template<Arithmetic U>
    constexpr Point2<T>& operator-=(const Vector2<U>& v)
    {
        HCheckNaNs(v);
		m_x -= v.m_x;
		m_y -= v.m_y;
		return *this;
    }

    template<Arithmetic U>
    constexpr auto operator-(const Vector2<U>& v) -> Point2<decltype(T{} - U{})>
    {
        Point2<decltype(T{} - U{}) > result = *this;
        result -= v;
        return result;
    }

    template<Arithmetic U>
    constexpr auto operator-(const Point2<U>& p)->Vector2<decltype(T{} - U{})>
    {
        HCheckNaNs(p);
        Vector2<decltype(T{} - U{}) > result{ m_x - p.m_x, m_y - p.m_y };
        return result;
    }
    // InvertBilinear remaining.
};

export template<Arithmetic T>
class Point3 : public Tuple3<Point3, T>
{
public:
    using Tuple3<Point3, T>::m_x;
    using Tuple3<Point3, T>::m_y;
    using Tuple3<Point3, T>::m_z;
    using Tuple3<Point3, T>::operator+;
    using Tuple3<Point3, T>::operator+=;
    using Tuple3<Point3, T>::operator*;
    using Tuple3<Point3, T>::operator*=;

    constexpr explicit Point3() noexcept : Tuple3<Point3, T>() {}
    constexpr Point3(T x, T y, T z) noexcept : Tuple3<Point3, T>(x, y, z) {}

    template<Arithmetic U>
    constexpr explicit Point3(const Vector3<U>& v) noexcept : Tuple3<Point3, T>(T(v.m_x), T(v.m_y), T(v.m_z)) {}

    constexpr Point3<T> operator-() const { return { -m_x, -m_y, -m_z }; }

    template<Arithmetic U>
    constexpr Point3<T>& operator+=(const Vector3<U>& v)
    {
        HCheckNaNs(v);
        m_x += v.m_x;
        m_y += v.m_y;
        m_z += v.m_z;
        return *this;
    }

    template<Arithmetic U>
    constexpr auto operator+(const Vector3<U>& v) const->Point3<decltype(T{} + U{}) >
    {
        Point3<decltype(T{} + U{}) > result = *this;
        result += v;
        return result;
    }

    template<Arithmetic U>
    constexpr Point3<T>& operator-=(const Vector3<U>& v)
    {
        HCheckNaNs(v);
        m_x -= v.m_x;
        m_y -= v.m_y;
        m_z -= v.m_z;
        return *this;
    }

    template<Arithmetic U>
    constexpr auto operator-(const Vector3<U>& v)->Point3<decltype(T{} - U{}) >
    {
        Point3<decltype(T{} - U{}) > result = *this;
        result -= v;
        return result;
    }

    template<Arithmetic U>
    constexpr auto operator-(const Point3<U>& p) const->Vector3<decltype(T{} - U{}) > {
        HCheckNaNs(p);
        Vector3<decltype(T{} - U{}) > result{ m_x - p.m_x, m_y - p.m_y, m_z - p.m_z };
        return result;
    }
};

export
{
    using Point2f = Point2<Float32>;
    using Point2d = Point2<Float64>;
    using Point2i = Point2<int>;

    // Point2c: Current precision: double / Float64 / Float.
    using Point2c = Point2d;


    using Point3f = Point3<Float32>;
    using Point3d = Point3<Float64>;
    using Point3i = Point3<int>;

    using Point3c = Point3d;
};