import color;
import interval;

import <algorithm>;
import <cmath>;

Color::Color(double x) noexcept : m_colorVec{ x, x, x } {}

Color::Color(double r, double g, double b) noexcept : m_colorVec{ r, g, b } {}

Color::Color(const Vec3& other) noexcept : m_colorVec{ other } {}

Vec3 Color::getBaseVec() const noexcept
{
	return m_colorVec;
}

Color Color::convertFromNormalized() const
{
	const Vec3 gammaCorrected{ convertLinearToGamma() };
	static const Interval colorIntensity(0.000, 0.999);
	return Color(((256 * colorIntensity.clampWithin(gammaCorrected[0]))), ((256 * colorIntensity.clampWithin(gammaCorrected[1]))), ((256 * colorIntensity.clampWithin(gammaCorrected[2]))));
}

Color Color::operator+(const Color& rhs) const noexcept
{
	return Color(m_colorVec[0] + rhs.m_colorVec[0], m_colorVec[1] + rhs.m_colorVec[1], m_colorVec[2] + rhs.m_colorVec[2]);
}

Color Color::operator*(const double scalar) const noexcept
{
	return Color(m_colorVec[0] * scalar, m_colorVec[1] * scalar, m_colorVec[2] * scalar);
}

Color Color::operator*(const Color& other) const noexcept
{
	return Color(m_colorVec * other.getBaseVec());
}

Vec3 Color::convertLinearToGamma() const noexcept
{
	return Color(std::fmax(0, std::sqrt(m_colorVec[0])), std::fmax(0, std::sqrt(m_colorVec[1])), std::fmax(0, std::sqrt(m_colorVec[2]))).getBaseVec();
}