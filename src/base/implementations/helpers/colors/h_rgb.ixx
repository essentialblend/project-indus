export module h_rgb;

import <compare>;
import <cmath>;
import <string>;
import <algorithm>;

import vector;

import h_mathutilities;

export class HRGB
{
public:
	constexpr explicit HRGB() noexcept = default;
	/*constexpr explicit HRGB(const Float x) noexcept : m_vecRGB{ x } {}
	constexpr explicit HRGB(const Float x, const Float y, const Float z) noexcept : m_vecRGB{ x, y, z } {}
	constexpr explicit HRGB(const Vec3c& other) noexcept : m_vecRGB{ other } {}

	constexpr HRGB(const HRGB&) = default;
	constexpr HRGB(HRGB&&) noexcept = default;
	constexpr HRGB& operator=(const HRGB&) = default;
	constexpr HRGB& operator=(HRGB&&) noexcept = default;
	
	constexpr double operator[](const std::size_t i) const noexcept { return m_vecRGB[i]; }
	constexpr double& operator[](const std::size_t i) noexcept { return m_vecRGB[i]; }

	HRGB& operator+=(const HRGB& other) noexcept { m_vecRGB += other.m_vecRGB; return *this; }
	HRGB& operator-=(const HRGB& other) noexcept { m_vecRGB -= other.m_vecRGB; return *this; }
	HRGB& operator*=(const Float scalar) noexcept { m_vecRGB *= scalar; return *this; }
	HRGB& operator/=(const Float scalar) noexcept { m_vecRGB *= (1.0 / scalar); return *this; }

	constexpr HRGB operator+(const HRGB& other) const noexcept { return HRGB(m_vecRGB + other.m_vecRGB); }
	constexpr HRGB operator-(const HRGB& other) const noexcept { return HRGB(m_vecRGB - other.m_vecRGB); }
	constexpr HRGB operator*(const Float scalar) const noexcept { return HRGB(m_vecRGB * scalar); }
	constexpr HRGB operator/(const Float scalar) const noexcept { return HRGB(m_vecRGB * (1.0 / scalar)); }

	constexpr HRGB toSRGB() const noexcept 
	{
		auto to_srgb_component{ [](Float c) -> Float {
			return c <= 0.0031308 ? 12.92 * c : 1.055 * std::pow(c, 1.0 / 2.4) - 0.055;
			} };
		return HRGB(to_srgb_component(m_vecRGB[0]), to_srgb_component(m_vecRGB[1]), to_srgb_component(m_vecRGB[2]));
	}

	constexpr HRGB fromSRGB() const noexcept
	{
		auto from_srgb_component{ [](Float c) -> Float {
			return c <= 0.04045 ? c / 12.92 : std::pow((c + 0.055) / 1.055, 2.4);
			} };
		return HRGB(from_srgb_component(m_vecRGB[0]), from_srgb_component(m_vecRGB[1]), from_srgb_component(m_vecRGB[2]));
	}

	constexpr std::string toString() const
	{
		return "RGB(" + std::to_string(m_vecRGB[0]) + ", " + std::to_string(m_vecRGB[1]) + ", " + std::to_string(m_vecRGB[2]) + ")";
	}

	constexpr void clamp() noexcept
	{
		m_vecRGB[0] = std::max(0.0, std::min(m_vecRGB[0], 1.0));
		m_vecRGB[1] = std::max(0.0, std::min(m_vecRGB[1], 1.0));
		m_vecRGB[2] = std::max(0.0, std::min(m_vecRGB[2], 1.0));
	}

	constexpr Float luminance() const noexcept
	{
		return 0.2126 * m_vecRGB[0] + 0.7152 * m_vecRGB[1] + 0.0722 * m_vecRGB[2];
	}*/

	~HRGB() noexcept = default;	

private:
	Vec3c m_vecRGB{};
};