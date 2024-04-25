export module color;

import vec3;

export class Color
{
public:
	explicit Color() = default;
	explicit Color(double x) noexcept;
	explicit Color(double r, double g, double b) noexcept;

	[[nodiscard]] Color operator+(const Color& rhs) const noexcept;

	[[nodiscard]] Color convertFromNormalized() const;
	[[nodiscard]] const Vec3 getBaseVec() const noexcept;

private:
	Vec3 m_colorVec{};
};