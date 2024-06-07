import eu_colorrgb;

import <cmath>;

EUColorRGB::EUColorRGB(const double x) noexcept : m_colorVec{ x } {}

EUColorRGB::EUColorRGB(const double x, const double y, const double z) noexcept : m_colorVec{ x, y, z } {}

EUColorRGB::EUColorRGB(const glm::dvec3& colorVec) noexcept : m_colorVec{ colorVec } {}

void EUColorRGB::addTo(const EUColor& otherColor)
{
	const auto& rgb{ static_cast<const EUColorRGB&>(otherColor) };
	m_colorVec += rgb.m_colorVec;
}

void EUColorRGB::multiplyWith(const EUColor& otherColor)
{
	const auto& rgb{ static_cast<const EUColorRGB&>(otherColor) };
	m_colorVec *= rgb.m_colorVec;
}

void EUColorRGB::multiplyScalarWith(const double x)
{
	m_colorVec *= x;
}

void EUColorRGB::negate() noexcept
{
	m_colorVec = -m_colorVec;
}

void EUColorRGB::gammaCorrect(const double x)
{
	for (glm::length_t i{}; i < 3; ++i)
	{
		if (m_colorVec[i] > 0)
		{
			m_colorVec[i] = std::pow(m_colorVec[i], 1.0 / x);
		}
	}
}

const EUColor& EUColorRGB::getColor()
{
	return *this;
}

void EUColorRGB::setColor(const EUColor& otherColor)
{
	const auto& rgb{ static_cast<const EUColorRGB&>(otherColor) };
	m_colorVec = rgb.m_colorVec;
}

double EUColorRGB::getRedComponent() const noexcept
{
	return m_colorVec[0];
}

double EUColorRGB::getGreenComponent() const noexcept
{
	return m_colorVec[1];
}

double EUColorRGB::getBlueComponent() const noexcept
{
	return m_colorVec[2];
}
