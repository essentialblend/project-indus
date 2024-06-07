export module vec3;

import core_util;

import <array>;
import <cmath>;
import <iostream>;

export class Vec3
{
public:
	// Constructors
	explicit Vec3() noexcept = default;
	explicit Vec3(const double x) noexcept;
	explicit Vec3(const double x, const double y, const double z) noexcept;

	// Default copy/move constructors/assignment ops.
	Vec3(const Vec3& other) noexcept = default;
	Vec3(Vec3&& other) noexcept = default;
	Vec3& operator=(const Vec3& other) noexcept = default;
	Vec3& operator=(Vec3&& other) noexcept = default;

	// Op overloads.
	Vec3& operator+=(const Vec3& other) noexcept;
	Vec3& operator-=(const Vec3& other) noexcept;
	Vec3& operator*=(const Vec3& other) noexcept;
	Vec3& operator*=(const double scalar) noexcept;
	Vec3 operator+(const Vec3& other) const noexcept;
	Vec3 operator-(const Vec3& other) const noexcept;
	Vec3 operator*(const Vec3& other) const noexcept;
	Vec3 operator*(const double scalar) const noexcept;
	Vec3 operator/(const double scalar) const;

	const double& operator[](const std::size_t index) const noexcept;
	double& operator[](const std::size_t index) noexcept;
	Vec3 operator-() const noexcept;

	// std::partial_ordering operator<=>(const Vec3& other) const noexcept;

	double getMagnitude() const;
	double getMagnitudeSq() const noexcept;
	bool isNearZero() const;

	~Vec3() noexcept = default;

private:
	std::array<double, 3> m_vector{};
};

// Non-member vec3 utils.
export
{
	[[nodiscard]] Vec3 operator*(const double scalar, const Vec3& other) noexcept;

	[[nodiscard]] double computeDot(const Vec3& f, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 computeCross(const Vec3& f, const Vec3& s) noexcept;
	[[nodiscard]] Vec3 getUnit(const Vec3& inputVec) noexcept;

	[[nodiscard]] Vec3 genRandomVec(double min = 0, double max = 1);
	[[nodiscard]] Vec3 genRandomUnitSphereVec();
	[[nodiscard]] Vec3 genRandomUnitSphereVecNorm();
	[[nodiscard]] Vec3 genRandomUnitHemisphereVecNorm(const Vec3& normalVec);
	[[nodiscard]] Vec3 genRandomUnitDiskVec();

	using PointOLD = Vec3;
}