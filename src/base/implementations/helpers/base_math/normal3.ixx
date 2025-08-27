export module normal3;

import h_mathutilities;
import vectors;

export template <Arithmetic T>
class Normal3 : public Tuple3<Normal3, T>
{
public:
    using value_type = T;
    static constexpr size_t numDimensions = 3;
    using Tuple3<Normal3, T>::m_x;
    using Tuple3<Normal3, T>::m_y;
    using Tuple3<Normal3, T>::m_z;
    using Tuple3<Normal3, T>::operator+;
    using Tuple3<Normal3, T>::operator*;
    using Tuple3<Normal3, T>::operator*=;

    constexpr explicit Normal3() noexcept : Tuple3<Normal3, T>() {}
    constexpr Normal3(T x, T y, T z) noexcept : Tuple3<Normal3, T>(x, y, z) {}

    template <Arithmetic U>
    constexpr explicit Normal3(const Normal3<U>& v) noexcept : Tuple3<Normal3, T>(T(v.m_x), T(v.m_y), T(v.m_z)) {}

    template <Arithmetic U>
    constexpr explicit Normal3(const Vector3<U>& v) noexcept : Tuple3<Normal3, T>(T(v.m_x), T(v.m_y), T(v.m_z)) {}
};

export
{
    using Normal3f = Normal3<Float32>;
    using Normal3d = Normal3<Float64>;
    using Normal3i = Normal3<int>;

    using Normal3c = Normal3d;
};