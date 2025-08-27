import vec3;

import core_util;

import <array>;
import <cmath>;
import <iostream>;

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

Vec3 Vec3::operator/(double scalar) const noexcept
{
	const double inv{ 1.0 / scalar };
	return Vec3{ m_vector[0] * inv, m_vector[1] * inv, m_vector[2] * inv };
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

bool Vec3::isNearZero() const noexcept
{
	constexpr auto epsilon = 1e-12;
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
	const double m2 = inputVec.getMagnitudeSq();
	
	if (!(m2 > 0.0) || !std::isfinite(m2)) 
	{  
		return Vec3{ 0,0,1 };                     
	}
	
	const double inv = 1.0 / std::sqrt(m2);
	return inputVec * inv;
}

auto reflect(const Vec3& unitIncidentDir, const Vec3& unitNormalForCompare) noexcept -> Vec3
{
	return unitIncidentDir - (2.0 * computeDot(unitIncidentDir, unitNormalForCompare) * unitNormalForCompare);
}

void buildOrthonormalBasis(const Vec3& normalVec, Vec3& outTangentVec, Vec3& outBitangentVec, Vec3& outNormalVec)
{
	const Vec3 localUnitNormal{ getUnit(normalVec) };
	
	Vec3 localUnitTangent{};
	
	if (std::fabs(localUnitNormal[2]) < 0.999)
	{
		localUnitTangent = getUnit(Vec3{ -localUnitNormal[1], localUnitNormal[0], 0.0 });
	}
	else 
	{
		localUnitTangent = getUnit(Vec3{ 0.0, -localUnitNormal[2], localUnitNormal[1] });
	}
	Vec3 localUnitBitangent{ getUnit(computeCross(localUnitNormal, localUnitTangent)) };
	localUnitTangent = computeCross(localUnitBitangent, localUnitNormal);

	outTangentVec = localUnitTangent;
	outBitangentVec = localUnitBitangent;
	outNormalVec = localUnitNormal;
}
