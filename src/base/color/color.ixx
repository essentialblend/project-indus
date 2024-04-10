export module color;

import std;
import vec3;

export class Color
{
public:
	constexpr Color() = default;
	constexpr explicit Color(double x) noexcept;
	constexpr explicit Color(double r, double g, double b) noexcept;

	[[nodiscard]] Color convertNormToIntegral() const;
	[[nodiscard]] const Vec3 getBaseVec() const noexcept;

private:
	Vec3 m_colorVec{};
};