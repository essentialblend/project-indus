import vec3;

import core_util;

import <iostream>;

Vec3::Vec3(double x) noexcept : m_Vec{ x, x, x } {}
Vec3::Vec3(double x, double y, double z) noexcept : m_Vec{ x, y, z } {}

double Vec3::getX() const noexcept { return m_Vec[0]; }
double Vec3::getY() const noexcept { return m_Vec[1]; }
double Vec3::getZ() const noexcept { return m_Vec[2]; }

const double& Vec3::operator[](const std::size_t index) const noexcept
{
	return m_Vec[index];
}

double& Vec3::operator[](std::size_t index) noexcept {

	return const_cast<double&>(std::as_const(*this)[index]);
}

Vec3& Vec3::operator+=(const Vec3& otherVec) noexcept
{
	m_Vec[0] += otherVec[0];
	m_Vec[1] += otherVec[1];
	m_Vec[2] += otherVec[2];

	return *this;
}

Vec3& Vec3::operator*=(double scalar) noexcept
{
	m_Vec[0] *= scalar;
	m_Vec[1] *= scalar;
	m_Vec[2] *= scalar;

	return *this;
}

Vec3& Vec3::operator/=(double scalar) noexcept
{
	return ((*this) *= (1 / scalar));
}

Vec3 Vec3::operator-() const noexcept
{
	return Vec3(-m_Vec[0], -m_Vec[1], -m_Vec[2]);
}

double Vec3::getMagnitudeSq() const noexcept
{
	return m_Vec[0] * m_Vec[0] + m_Vec[1] * m_Vec[1] + m_Vec[2] * m_Vec[2];
}

double Vec3::getMagnitude() const noexcept
{
	return std::sqrt(getMagnitudeSq());
}

bool Vec3::isNearZero() const noexcept
{
	const auto eps{ std::numeric_limits<double>::epsilon() };
	return ((std::fabs(m_Vec[0]) < eps) && (std::fabs(m_Vec[1]) < eps) && (std::fabs(m_Vec[2]) < eps));
}


// Non-member vec3 utils.
std::ostream& operator<<(std::ostream& outStream, const Vec3& outVec)
{
	return outStream << outVec[0] << " " << outVec[1] << " " << outVec[2] << "\n";
}

Vec3 operator+(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[0] + s[0], f[1] + s[1], f[2] + s[2]);
}

Vec3 operator-(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[0] - s[0], f[1] - s[1], f[2] - s[2]);
}

Vec3 operator*(const Vec3& f, const Vec3& s) noexcept
{
	return Vec3(f[0] * s[0], f[1] * s[1], f[2] * s[2]);
}

Vec3 operator*(double t, const Vec3& s) noexcept
{
	return Vec3(t * s[0], t * s[1], t * s[2]);
}

Vec3 operator*(const Vec3& f, double t) noexcept
{
	return t * f;
}

Vec3 operator/(const Vec3& f, double t) noexcept
{
	return (1 / t) * f;
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
	return inputVec / inputVec.getMagnitude();
}

Vec3 genRandomVec(double min, double max) noexcept
{
	return Vec3(UGenRNG(min, max), UGenRNG(min, max), UGenRNG(min, max));
}

Vec3 genRandomUnitSphereVec()
{
	while (true)
	{
		auto randomVec = genRandomVec(-1, 1);
		if (randomVec.getMagnitudeSq() < 1) 
			return randomVec;
	}
}

Vec3 genRandomUnitSphereVecNorm()
{
	return getUnit(genRandomUnitSphereVec());
}

Vec3 genRandomUnitHemisphereVecNorm(const Vec3& normalVec)
{
	Vec3 unitSphereRandVec{ genRandomUnitSphereVecNorm() };
	if (computeDot(unitSphereRandVec, normalVec) > 0.0)
		return unitSphereRandVec;
	else
		return -unitSphereRandVec;
}

