import vec3;

import <array>;
import <cmath>;
import <iostream>;

import core_util;

// Member functions.
Vec3::Vec3(double x) noexcept : m_vector{ x, x, x } {}
Vec3::Vec3(double x, double y, double z) noexcept : m_vector{ x, y, z } {}

Vec3& Vec3::operator+=(const Vec3& other) noexcept
{
	m_vector[0] += other[0];
	m_vector[1] += other[1];
	m_vector[2] += other[2];

	return *this;
}

Vec3& Vec3::operator-=(const Vec3& other) noexcept
{
	m_vector[0] -= other[0];
	m_vector[1] -= other[1];
	m_vector[2] -= other[2];

	return *this;
}

Vec3& Vec3::operator*=(const Vec3& other) noexcept
{
	m_vector[0] *= other[0];
	m_vector[1] *= other[1];
	m_vector[2] *= other[2];

	return *this;
}

Vec3& Vec3::operator*=(const double scalar) noexcept
{
	m_vector[0] *= scalar;
	m_vector[1] *= scalar;
	m_vector[2] *= scalar;

	return *this;
}

Vec3 Vec3::operator+(const Vec3& other) const noexcept
{
	return Vec3(m_vector[0] + other.m_vector[0], m_vector[1] + other.m_vector[1], m_vector[2] + other.m_vector[2]);
}

Vec3 Vec3::operator-(const Vec3& other) const noexcept
{
	return Vec3(m_vector[0] - other.m_vector[0], m_vector[1] - other.m_vector[1], m_vector[2] - other.m_vector[2]);
}

Vec3 Vec3::operator*(const Vec3& other) const noexcept
{
	return Vec3(m_vector[0] * other.m_vector[0], m_vector[1] * other.m_vector[1], m_vector[2] * other.m_vector[2]);
}

Vec3 Vec3::operator*(double scalar) const noexcept
{
	return Vec3(m_vector[0] * scalar, m_vector[1] * scalar, m_vector[2] * scalar);
}

Vec3 Vec3::operator/(double scalar) const
{
	if ((1 / scalar) == 0)
	{
		throw std::runtime_error("Division by zero. Exiting.");
	}
	return Vec3(m_vector[0] / scalar, m_vector[1] / scalar, m_vector[2] / scalar);
}

const double& Vec3::operator[](const std::size_t index) const noexcept
{
	return m_vector[index];
}

double& Vec3::operator[](const std::size_t index) noexcept
{
	return const_cast<double&>(std::as_const(*this)[index]);
}

Vec3 Vec3::operator-() const noexcept
{
	return Vec3(-m_vector[0], -m_vector[1], -m_vector[2]);
}

double Vec3::getMagnitude() const
{
	return std::sqrt(getMagnitudeSq());
}

double Vec3::getMagnitudeSq() const noexcept
{
	return m_vector[0] * m_vector[0] + m_vector[1] * m_vector[1] + m_vector[2] * m_vector[2];
}

bool Vec3::isNearZero() const
{
	constexpr auto epsilon = std::numeric_limits<double>::epsilon();
	return (std::fabs(m_vector[0]) < epsilon) && (std::fabs(m_vector[1]) < epsilon) && (std::fabs(m_vector[2]) < epsilon);
}

// Non-member functions and vec3 utils.

Vec3 operator*(double scalar, const Vec3& other) noexcept
{
	return Vec3(other[0] * scalar, other[1] * scalar, other[2] * scalar);
}

double computeDot(const Vec3& f, const Vec3& s) noexcept
{
	return (f[0] * s[0]) + (f[1] * s[1]) + (f[2] * s[2]);
}

Vec3 computeCross(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[1] * s[2] - f[2] * s[1],
		f[2] * s[0] - f[0] * s[2],
		f[0] * s[1] - f[1] * s[0]);
}

Vec3 getUnit(const Vec3& inputVec) noexcept
{
	if (inputVec.isNearZero())
	{
		std::cerr << "Fatal: Normalizing a zero vector resulting in division by zero. Exiting." << std::endl;
		std::exit(1);
	}
	return inputVec / inputVec.getMagnitude();
}

Vec3 genRandomVec(double min, double max)
{
	return Vec3(UGenRNG(min, max), UGenRNG(min, max), UGenRNG(min, max));
}

Vec3 genRandomUnitSphereVec()
{
	while (true)
	{
		const auto randomVec = genRandomVec(-1, 1);
		if (randomVec.getMagnitudeSq() < 1)
			return Vec3(randomVec);
	}
}

Vec3 genRandomUnitSphereVecNorm()
{
	return getUnit(genRandomUnitSphereVec());
}

Vec3 genRandomUnitHemisphereVecNorm(const Vec3& normalVec)
{
	const Vec3 unitSphereRandVec{ genRandomUnitSphereVecNorm() };
	if (computeDot(unitSphereRandVec, normalVec) > 0.0)
		return Vec3(unitSphereRandVec);
	else
		return -Vec3(unitSphereRandVec);
}

Vec3 genRandomUnitDiskVec()
{
	while (true)
	{
		const auto randomVec = Vec3(UGenRNG<double>(-1, 1), UGenRNG<double>(-1, 1), 0);
		if (randomVec.getMagnitudeSq() < 1)
			return Vec3(randomVec);
	}
}