export module h_xyz;

import <compare>;
import <cmath>;
import <string>;
import <algorithm>;

import vector;

import h_mathutilities;

export class HXYZ 
{
public:
    constexpr explicit HXYZ() noexcept = default;
    constexpr explicit HXYZ(const Float x) noexcept : m_vecXYZ{ x, x, x } {}
    constexpr explicit HXYZ(const Float x, const Float y, const Float z) noexcept : m_vecXYZ{ x, y, z } {}
    constexpr explicit HXYZ(const Vec3c& other) noexcept : m_vecXYZ{ other } {}

    constexpr HXYZ(const HXYZ&) = default;
    constexpr HXYZ(HXYZ&&) noexcept = default;
    constexpr HXYZ& operator=(const HXYZ&) = default;
    constexpr HXYZ& operator=(HXYZ&& other) noexcept = default;

    constexpr Float operator[](const std::size_t i) const noexcept { return m_vecXYZ[i]; }
    constexpr Float& operator[](const std::size_t i) noexcept { return m_vecXYZ[i]; }

    HXYZ& operator+=(const HXYZ& other) noexcept { m_vecXYZ += other.m_vecXYZ; return *this; }
    HXYZ& operator-=(const HXYZ& other) noexcept { m_vecXYZ -= other.m_vecXYZ; return *this; }
    HXYZ& operator*=(const Float scalar) noexcept { m_vecXYZ *= scalar; return *this; }
    HXYZ& operator/=(const Float scalar) noexcept { m_vecXYZ /= scalar; return *this; }

    constexpr HXYZ operator+(const HXYZ& other) const noexcept { return HXYZ(m_vecXYZ + other.m_vecXYZ); }
    constexpr HXYZ operator-(const HXYZ& other) const noexcept { return HXYZ(m_vecXYZ - other.m_vecXYZ); }
    constexpr HXYZ operator*(const Float scalar) const noexcept { return HXYZ(m_vecXYZ * scalar); }
    constexpr HXYZ operator/(const Float scalar) const noexcept { return HXYZ(m_vecXYZ / scalar); }

    constexpr std::string ToString() const {
        return "XYZ(" + std::to_string(m_vecXYZ[0]) + ", " + std::to_string(m_vecXYZ[1]) + ", " + std::to_string(m_vecXYZ[2]) + ")";
    }

    constexpr void clamp() noexcept {
        m_vecXYZ[0] = std::max(0.0, std::min(m_vecXYZ[0], 1.0));
        m_vecXYZ[1] = std::max(0.0, std::min(m_vecXYZ[1], 1.0));
        m_vecXYZ[2] = std::max(0.0, std::min(m_vecXYZ[2], 1.0));
    }

    ~HXYZ() noexcept = default;

private:
    Vec3c m_vecXYZ;
};