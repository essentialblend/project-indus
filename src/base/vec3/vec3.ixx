export module vec3;

import <iostream>;
import <array>;

export class Vec3
{
public:
	explicit Vec3() = default;

	explicit Vec3(double x) noexcept;
	explicit Vec3(double x, double y, double z) noexcept;

	[[nodiscard]] double getX() const noexcept;
	[[nodiscard]] double getY() const noexcept;
	[[nodiscard]] double getZ() const noexcept;

	[[nodiscard]] const double& operator[](std::size_t index) const noexcept;
	double& operator[](std::size_t index) noexcept;

	Vec3& operator+=(const Vec3& otherVec) noexcept;
	Vec3& operator*=(double scalar) noexcept;
	Vec3& operator/=(double scalar) noexcept;

	Vec3 operator-() const noexcept;

	[[nodiscard]] double getMagnitude() const noexcept;
	[[nodiscard]] double getMagnitudeSq() const noexcept;

private:
	std::array<double, 3> m_Vec{};
};

// Non-member vec3 utils.
export {

	std::ostream& operator<<(std::ostream& outStream, const Vec3& outVec);

	[[nodiscard]] Vec3 operator+(const Vec3& f, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 operator-(const Vec3& f, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 operator*(const Vec3& f, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 operator*(double t, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 operator*(const Vec3& f, double t) noexcept;
	[[nodiscard]] Vec3 operator/(const Vec3& f, double t) noexcept;

	[[nodiscard]] double computeDot(const Vec3& f, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 computeCross(const Vec3& f, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 getUnit(const Vec3& inputVec) noexcept;

	using Point = Vec3;
}