import color;

Color::Color(double x) noexcept : m_colorVec{ x, x, x } {}

Color::Color(double r, double g, double b) noexcept : m_colorVec{ r, g, b } {}

const Vec3 Color::getBaseVec() const noexcept
{
	return m_colorVec;
}

Color Color::convertNormToIntegral() const
{
	return Color(static_cast<std::uint8_t>(255.999 * m_colorVec[0]), static_cast<std::uint8_t>(255.999 * m_colorVec[1]), static_cast<std::uint8_t>(255.999 * m_colorVec[2]));
}