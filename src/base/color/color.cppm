import color;
import interval;

Color::Color(double x) noexcept : m_colorVec{ x, x, x } {}

Color::Color(double r, double g, double b) noexcept : m_colorVec{ r, g, b } {}

const Vec3 Color::getBaseVec() const noexcept
{
	return m_colorVec;
}

Color Color::convertFromNormalized() const
{
	static const Interval colorIntensity(0.000, 0.999);
	return Color(((256 * colorIntensity.clampWithin(m_colorVec[0]))), ((256 * colorIntensity.clampWithin(m_colorVec[1]))), ((256 * colorIntensity.clampWithin(m_colorVec[2]))));
}

Color Color::operator+(const Color& rhs) const noexcept
{
	return Color(m_colorVec[0] + rhs.m_colorVec[0], m_colorVec[1] + rhs.m_colorVec[1], m_colorVec[2] + rhs.m_colorVec[2]);
}