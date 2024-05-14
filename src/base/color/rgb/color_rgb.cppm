import color_rgb;

import <cmath>;
import <memory>;
import <stdexcept>;

import interval;

ColorRGB& ColorRGB::operator+=(const ColorRGB& other) noexcept
{
	m_colorVector += other.m_colorVector;
	return *this;
}

ColorRGB& ColorRGB::operator-=(const ColorRGB& other) noexcept
{
	m_colorVector -= other.m_colorVector;
	return *this;
}

ColorRGB& ColorRGB::operator*=(const ColorRGB& other) noexcept
{
	m_colorVector *= other.m_colorVector;
	return *this;
}

ColorRGB& ColorRGB::operator*=(const double scalar) noexcept
{
	m_colorVector *= scalar;
	return *this;
}

ColorRGB ColorRGB::operator+(const ColorRGB& other) const noexcept
{
	return ColorRGB(m_colorVector + other.m_colorVector);
}

ColorRGB ColorRGB::operator-(const ColorRGB& other) const noexcept
{
	return ColorRGB(m_colorVector - other.m_colorVector);
}

ColorRGB ColorRGB::operator*(const ColorRGB& other) const noexcept
{
	return ColorRGB(m_colorVector * other.m_colorVector);
}

ColorRGB ColorRGB::operator*(const double scalar) const noexcept
{
	return ColorRGB(m_colorVector * scalar);
}

ColorRGB ColorRGB::operator/(const double scalar) const
{
	if ((1 / scalar) == 0)
	{
		throw std::invalid_argument("Division by zero");
	}

	return ColorRGB(m_colorVector / scalar);
}

void ColorRGB::undoNormalization(const double normFactor)
{
	static const Interval colorIntensity(0.000, 0.999);

	for (std::size_t i{}; i < 3; ++i)
	{
		m_colorVector[i] = normFactor * colorIntensity.clampWithin(m_colorVector[i]);
	}
}

void ColorRGB::addColorToSelf(const IColor& otherColor)
{
	const auto& otherColorRGB{ static_cast<const ColorRGB&>(otherColor) };
	m_colorVector += otherColorRGB.m_colorVector;

}

void ColorRGB::multiplyColorWithSelf(const IColor& otherColor)
{
	const auto& otherColorRGB{ static_cast<const ColorRGB&>(otherColor) };
	m_colorVector *= otherColorRGB.m_colorVector;
}

void ColorRGB::negateSelf() noexcept
{
	for (std::size_t i{}; i < 3; ++i)
	{
		if (m_colorVector[i] != 0)
		{
			m_colorVector[i] = -m_colorVector[i];
		}
	}
}


std::shared_ptr<const IColor> ColorRGB::getColor() const
{
	return std::static_pointer_cast<const IColor>(shared_from_this());
}

void ColorRGB::setColor(const IColor& otherColor)
{
	const auto& otherColorRGB{ static_cast<const ColorRGB&>(otherColor) };
	m_colorVector = otherColorRGB.m_colorVector;
}

void ColorRGB::applyWeights(const double weightFactor) noexcept
{
	for (std::size_t i{}; i < 3; ++i)
	{
		m_colorVector[i] *= (1.0 / weightFactor);
	}
}

void ColorRGB::applyNormalization()
{
	for (std::size_t i{}; i < 3; ++i)
	{
		m_colorVector[i] *= (1.0 / 256);
	}
}

void ColorRGB::applyGammaCorrection(const double gammaFactor)
{
    for (std::size_t i{}; i < 3; ++i)
    {
        if (m_colorVector[i] > 0)
        {
            m_colorVector[i] = std::pow(m_colorVector[i], 1.0 / gammaFactor);
        }
    }
}

void ColorRGB::transformSelfToMinValues() noexcept
{
	for (std::size_t i{}; i < 3; ++i)
	{
		m_colorVector[i] = 0.0;
	}
}

void ColorRGB::transformSelfToMaxValues() noexcept
{
	for (std::size_t i{}; i < 3; ++i)
	{
		m_colorVector[i] = 1.0;
	}
}

double ColorRGB::getRedComponent() const noexcept
{
	return m_colorVector[0];
}

double ColorRGB::getGreenComponent() const noexcept
{
	return m_colorVector[1];
}

double ColorRGB::getBlueComponent() const noexcept
{
	return m_colorVector[2];
}