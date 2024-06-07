export module point3;

import <compare>;
import <type_traits>;
import <intrin.h>;
import <array>;
import <iostream>;
import <cmath>;

import vector;
import h_mathutilities;
import h_miscutilities;

export template<typename T>
class alignas(32) Point3
{
//public:
//    constexpr explicit Point3() noexcept = default;
//    constexpr explicit Point3(const T) noexcept;
//    constexpr explicit Point3(const T, const T, const T) noexcept;
//    constexpr explicit Point3(const Vector3<T>&) noexcept;
//
//    constexpr Point3(const Point3&) = default;
//    constexpr Point3& operator=(const Point3&) = default;
//    constexpr Point3(Point3&&) noexcept = default;
//    constexpr Point3& operator=(Point3&&) noexcept = default;
//
//    constexpr auto operator<=>(const Point3&) const = default;
//    constexpr bool operator==(const Point3&) const = default;
//    constexpr bool operator!=(const Point3&) const = default;
//
//    [[nodiscard]] constexpr T operator[](const size_t) const;
//
//
//    [[nodiscard]] constexpr Point3 operator+(const Vector3<T>&) const;
//    [[nodiscard]] constexpr Point3 operator-(const Vector3<T>&) const;
//    [[nodiscard]] constexpr Vector3<T> operator-(const Point3&) const;
//    constexpr Point3& operator+=(const Vector3<T>&);
//    constexpr Point3& operator-=(const Vector3<T>&);
//    [[nodiscard]] constexpr Point3 operator*(const T) const;
//    [[nodiscard]] constexpr Point3 operator/(const T) const;
//    constexpr Point3& operator*=(const T);
//    constexpr Point3& operator/=(const T);
//
//    constexpr void prefetchForCacheUtilization() const noexcept;
//
//    ~Point3() noexcept = default;
private:
    T m_x{}, m_y{}, m_z{};
};
//
//// Non-members.
//
//export template<typename T>
//[[nodiscard]] constexpr Point3<T> operator*(const T, const Point3<T>&);
//
//export template<typename T>
//[[nodiscard]] constexpr T distance(const Point3<T>&, const Point3<T>&);
//
//export template<typename T>
//[[nodiscard]] constexpr T distanceSq(const Point3<T>&, const Point3<T>&);
//
//export template<typename T>
//[[nodiscard]] constexpr Point3<T> lerp(const T, const Point3<T>&, const Point3<T>&);
//
export
{
    using Point3f = Point3<Float32>;
    using Point3d = Point3<Float64>;
    using Point3i = Point3<int>;

    using Point3c = Point3d;
};
//
//// Implementation must be in the interface because we need to export the template in its entirety.
//
//template<typename T>
//constexpr Point3<T>::Point3(const T x) noexcept : m_x{ x }, m_y{ x }, m_z{ x } {}
//
//template<typename T>
//constexpr Point3<T>::Point3(const T x, const T y, const T z) noexcept : m_x{ x }, m_y{ y }, m_z{ z } {}
//
//template<typename T>
//constexpr Point3<T>::Point3(const Vector3<T>& vec) noexcept : m_x{ vec[0] }, m_y{ vec[1] }, m_z{ vec[2] } {}
//
//template<typename T>
//constexpr T Point3<T>::operator[](const size_t index) const
//{
//    return (index == 0) ? m_x : (index == 1) ? m_y : m_z;
//}
//
//template<typename T>
//constexpr Point3<T> Point3<T>::operator+(const Vector3<T>& vec) const
//{
//    Point3<T> result{ *this };
//    result += vec;
//    return result;
//}
//
//template<typename T>
//constexpr Point3<T> Point3<T>::operator-(const Vector3<T>& vec) const
//{
//    Point3<T> result{ *this };
//    result -= vec;
//    return result;
//}
//
//template<typename T>
//constexpr Vector3<T> Point3<T>::operator-(const Point3<T>& point) const
//{
//    return Vector3<T>{ m_x - point.m_x, m_y - point.m_y, m_z - point.m_z };
//}
//
//template<typename T>
//constexpr Point3<T>& Point3<T>::operator+=(const Vector3<T>& vec)
//{
//    bool needFallback{ true };
//
//    this->prefetchForCacheUtilization();
//    vec.prefetchForCacheUtilization();
//
//    if constexpr (std::is_same_v<T, float>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m128 first{ _mm_set_ps(0.0f, m_z, m_y, m_x) };
//            __m128 second{ _mm_set_ps(0.0f, vec[2], vec[1], vec[0]) };
//            __m128 add{ _mm_add_ps(first, second) };
//
//            _mm_store_ps(result.data(), add);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//    else if constexpr (std::is_same_v<T, double>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m256d first{ _mm256_set_pd(0.0, m_z, m_y, m_x) };
//            __m256d second{ _mm256_set_pd(0.0, vec[2], vec[1], vec[0]) };
//            __m256d add{ _mm256_add_pd(first, second) };
//
//            _mm256_store_pd(result.data(), add);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//
//    if (needFallback)
//    {
//        m_x += vec[0];
//        m_y += vec[1];
//        m_z += vec[2];
//    }
//
//    return *this;
//}
//
//template<typename T>
//constexpr Point3<T>& Point3<T>::operator-=(const Vector3<T>& vec)
//{
//    bool needFallback{ true };
//
//    this->prefetchForCacheUtilization();
//    vec.prefetchForCacheUtilization();
//
//    if constexpr (std::is_same_v<T, float>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m128 first{ _mm_set_ps(0.0f, m_z, m_y, m_x) };
//            __m128 second{ _mm_set_ps(0.0f, vec[2], vec[1], vec[0]) };
//            __m128 sub{ _mm_sub_ps(first, second) };
//
//            _mm_store_ps(result.data(), sub);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//    else if constexpr (std::is_same_v<T, double>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m256d first{ _mm256_set_pd(0.0, m_z, m_y, m_x) };
//            __m256d second{ _mm256_set_pd(0.0, vec[2], vec[1], vec[0]) };
//            __m256d sub{ _mm256_sub_pd(first, second) };
//
//            _mm256_store_pd(result.data(), sub);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//
//    if (needFallback)
//    {
//        m_x -= vec[0];
//        m_y -= vec[1];
//        m_z -= vec[2];
//    }
//
//    return *this;
//}
//
//template<typename T>
//constexpr Point3<T> Point3<T>::operator*(const T scalar) const
//{
//    Point3<T> result{ *this };
//    result *= scalar;
//    return result;
//}
//
//template<typename T>
//constexpr Point3<T> Point3<T>::operator/(const T scalar) const
//{
//    Point3<T> result{ *this };
//    result /= scalar;
//    return result;
//}
//
//template<typename T>
//constexpr Point3<T>& Point3<T>::operator*=(const T scalar)
//{
//    bool needFallback{ true };
//
//    this->prefetchForCacheUtilization();
//
//    if constexpr (std::is_same_v<T, float>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m128 vec{ _mm_set_ps(0.0f, m_z, m_y, m_x) };
//            __m128 sca{ _mm_set1_ps(scalar) };
//            __m128 mul{ _mm_mul_ps(vec, sca) };
//
//            _mm_store_ps(result.data(), mul);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//    else if constexpr (std::is_same_v<T, double>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m256d vec{ _mm256_set_pd(0.0, m_z, m_y, m_x) };
//            __m256d sca{ _mm256_set1_pd(scalar) };
//            __m256d mul{ _mm256_mul_pd(vec, sca) };
//
//            _mm256_store_pd(result.data(), mul);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//
//    if (needFallback)
//    {
//        m_x *= scalar;
//        m_y *= scalar;
//        m_z *= scalar;
//    }
//
//    return *this;
//}
//
//template<typename T>
//constexpr Point3<T>& Point3<T>::operator/=(const T scalar)
//{
//    if (scalar == 0) {
//        m_x = 0;
//        m_y = 0;
//        m_z = 0;
//        return *this;
//    }
//
//    bool needFallback{ true };
//
//    this->prefetchForCacheUtilization();
//
//    if constexpr (std::is_same_v<T, float>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m128 vec{ _mm_set_ps(0.0f, m_z, m_y, m_x) };
//            __m128 sca{ _mm_set1_ps(scalar) };
//            __m128 div{ _mm_div_ps(vec, sca) };
//
//            _mm_store_ps(result.data(), div);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//    else if constexpr (std::is_same_v<T, double>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            alignas(32) std::array<T, 4> result{};
//
//            __m256d vec{ _mm256_set_pd(0.0, m_z, m_y, m_x) };
//            __m256d sca{ _mm256_set1_pd(scalar) };
//            __m256d div{ _mm256_div_pd(vec, sca) };
//
//            _mm256_store_pd(result.data(), div);
//
//            m_x = result[0];
//            m_y = result[1];
//            m_z = result[2];
//
//            needFallback = false;
//        }
//    }
//
//    if (needFallback)
//    {
//        m_x /= scalar;
//        m_y /= scalar;
//        m_z /= scalar;
//    }
//
//    return *this;
//}
//
//template<typename T>
//constexpr void Point3<T>::prefetchForCacheUtilization() const noexcept
//{
//    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
//    {
//        _mm_prefetch(reinterpret_cast<const char*>(this), _MM_HINT_T0);
//    }
//}
//
//// Non-member Functions
//
//template<typename T>
//constexpr T distance(const Point3<T>& p1, const Point3<T>& p2)
//{
//    return std::sqrt(distanceSq(p1, p2));
//}
//
//template<typename T>
//constexpr T distanceSq(const Point3<T>& p1, const Point3<T>& p2)
//{
//    p1.prefetchForCacheUtilization();
//    p2.prefetchForCacheUtilization();
//
//    if constexpr (std::is_same_v<T, float>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            __m128 vec1 = _mm_set_ps(0.0f, p1[2], p1[1], p1[0]);
//            __m128 vec2 = _mm_set_ps(0.0f, p2[2], p2[1], p2[0]);
//
//            __m128 diff = _mm_sub_ps(vec1, vec2);
//            __m128 mul = _mm_mul_ps(diff, diff);
//            __m128 hadd = _mm_hadd_ps(mul, mul);
//            hadd = _mm_hadd_ps(hadd, hadd);
//
//            float result = _mm_cvtss_f32(hadd);
//            std::cout << "SIMD float distanceSq: " << result << std::endl;
//            return result;
//        }
//    }
//    else if constexpr (std::is_same_v<T, double>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            __m256d vec1 = _mm256_set_pd(0.0, p1[2], p1[1], p1[0]);
//            __m256d vec2 = _mm256_set_pd(0.0, p2[2], p2[1], p2[0]);
//
//            __m256d diff = _mm256_sub_pd(vec1, vec2);
//            __m256d mul = _mm256_mul_pd(diff, diff);
//            __m256d hadd = _mm256_hadd_pd(mul, mul);
//
//            double result = hadd[0] + hadd[2];
//            std::cout << "SIMD double distanceSq: " << result << std::endl;
//            return result;
//        }
//    }
//
//    T dx = p1[0] - p2[0];
//    T dy = p1[1] - p2[1];
//    T dz = p1[2] - p2[2];
//    T result = dx * dx + dy * dy + dz * dz;
//    std::cout << "Scalar distanceSq: " << result << std::endl;
//    return result;
//
//}
//
//template<typename T>
//constexpr Point3<T> lerp(const T t, const Point3<T>& p0, const Point3<T>& p1)
//{
//    p0.prefetchForCacheUtilization();
//    p1.prefetchForCacheUtilization();
//
//    if constexpr (std::is_same_v<T, float>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            __m128 vec0 = _mm_set_ps(0.0f, p0[2], p0[1], p0[0]);
//            __m128 vec1 = _mm_set_ps(0.0f, p1[2], p1[1], p1[0]);
//            __m128 scalar = _mm_set1_ps(t);
//
//            __m128 one_minus_t = _mm_sub_ps(_mm_set1_ps(1.0f), scalar);
//            __m128 result = _mm_add_ps(_mm_mul_ps(one_minus_t, vec0), _mm_mul_ps(scalar, vec1));
//
//            alignas(32) std::array<float, 4> resultArray{};
//            _mm_store_ps(resultArray.data(), result);
//
//            return Point3<T>{ resultArray[0], resultArray[1], resultArray[2] };
//        }
//    }
//    else if constexpr (std::is_same_v<T, double>)
//    {
//        if (HIsAVX2Enabled())
//        {
//            __m256d vec0 = _mm256_set_pd(0.0, p0[2], p0[1], p0[0]);
//            __m256d vec1 = _mm256_set_pd(0.0, p1[2], p1[1], p1[0]);
//            __m256d scalar = _mm256_set1_pd(t);
//
//            __m256d one_minus_t = _mm256_sub_pd(_mm256_set1_pd(1.0), scalar);
//            __m256d result = _mm256_add_pd(_mm256_mul_pd(one_minus_t, vec0), _mm256_mul_pd(scalar, vec1));
//
//            alignas(32) std::array<double, 4> resultArray{};
//            _mm256_store_pd(resultArray.data(), result);
//
//            return Point3<T>{ resultArray[0], resultArray[1], resultArray[2] };
//        }
//    }
//
//    Vector3<T> v0{ Vector3<T>{ p0[0], p0[1], p0[2] } };
//    Vector3<T> v1{ Vector3<T>{ p1[0], p1[1], p1[2] } };
//
//    return Point3<T>((v0 * (1 - t)) + (v1 * t));
//}
//
//template<typename T>
//[[nodiscard]] constexpr Point3<T> operator*(const T scalar, const Point3<T>& point)
//{
//    return point * scalar;
//}