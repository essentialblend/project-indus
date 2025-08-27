export module quaternion;

import h_mathutilities;
import vectors;

export class Quaternion
{
public:
    constexpr explicit Quaternion() noexcept : v(), w(1.0f) {}

    constexpr Quaternion(const Vector3<Float>& v, Float w) noexcept : v(v), w(w) {}

    constexpr Quaternion& operator+=(const Quaternion& q) noexcept
    {
        v += q.v;
        w += q.w;
        return *this;
    }

    constexpr Quaternion operator+(const Quaternion& q) const noexcept
    {
        Quaternion result{ *this };
        result += q;
        return result;
    }

    constexpr Quaternion& operator-=(const Quaternion& q) noexcept
    {
        v -= q.v;
        w -= q.w;
        return *this;
    }

    constexpr Quaternion operator-() const noexcept
    {
        return { -v, -w };
    }

    constexpr Quaternion operator-(const Quaternion& q) const noexcept
    {
        Quaternion result{ *this };
        result -= q;
        return result;
    }

    constexpr Quaternion& operator*=(Float f) noexcept
    {
        v *= f;
        w *= f;
        return *this;
    }

    constexpr Quaternion operator*(Float f) const noexcept
    {
        Quaternion result{ *this };
        result *= f;
        return result;
    }

    const Quaternion& operator/=(Float f)
    {
        HCheckDivByZero(f);
        v /= f;
        w /= f;
        return *this;
    }

    Quaternion operator/(Float f) const
    {
        HCheckDivByZero(f);
        Quaternion result{ *this };
        result /= f;
        return result;
    }

    // Quaternion Public Members
    Vec3c v;
    Float w;
};