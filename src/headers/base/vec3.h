#pragma once

class Vec3
{
public:
	Vec3() : member{ 0, 0, 0 } {}
	Vec3(double f) : member{ f, f, f} {}
	Vec3(double f, double s, double t) : member{ f, s, t } {}

	// Member getters and operators.
	double getX() const { return member[0]; }
	double getY() const { return member[1]; }
	double getZ() const { return member[2]; }

	Vec3 operator-() const { return Vec3(-member[0], -member[1], -member[2]); }
	double operator[](unsigned short i) const { return member[i]; }
	double& operator[](unsigned short i) { return member[i]; }
	
	Vec3& operator+=(const Vec3& inpV)
	{
		member[0] += inpV.getX();
		member[1] += inpV.getY();
		member[2] += inpV.getZ();
		return *this;
	}
	
	Vec3& operator*=(const Vec3& inpV)
	{
		member[0] *= inpV.getX();
		member[1] *= inpV.getY();
		member[2] *= inpV.getZ();
		return *this;
	}

	Vec3& operator/=(const double inpV)
	{
		return *this *= 1 / inpV;
	}

	double computeMagnitude() const
	{
		return std::sqrt(computeMagnitudeSquared());
	}

	double computeMagnitudeSquared() const
	{
		return (member[0] * member[0]) + (member[1] * member[1]) + (member[2] * member[2]);
	}
private:
	double member[3];
};

// Vector utilities.

using PointVec3 = Vec3;
using ColorVec3 = Vec3;

inline std::ostream& operator<<(std::ostream& outStream, const Vec3& inpV)
{
	return outStream << inpV[0] << ' ' << inpV[1] << ' ' << inpV[2];
}

inline Vec3 operator+(const Vec3& f, const Vec3& s)
{
	return Vec3(f.getX() + s.getX(), f.getY() + s.getY(), f.getZ() + s.getZ());
}

inline Vec3 operator-(const Vec3& f, const Vec3& s)
{
	return Vec3(f.getX() - s.getX(), f.getY() - s.getY(), f.getZ() - s.getZ());
}

inline Vec3 operator*(const Vec3& f, const Vec3& s)
{
	return Vec3(f.getX() * s.getX(), f.getY() * s.getY(), f.getZ() * s.getZ());
}

inline Vec3 operator*(const double t, const Vec3& s)
{
	return Vec3(t * s.getX(), t * s.getY(), t * s.getZ());
}

inline Vec3 operator*(const Vec3& f, const double t)
{
	return t * f;
}

inline Vec3 operator/(const Vec3& f, const double t)
{
	return (1 / t) * f;
}

inline double computeDotProduct(const Vec3& f, const Vec3& s)
{
	return f.getX() * s.getX() + f.getY() * s.getY() + f.getZ() * s.getZ();
}

inline Vec3 computeCrossProduct(const Vec3& f, const Vec3& s)
{
	return Vec3(f.getY() * s.getZ() - f.getZ() * s.getY(), f.getZ() * s.getX() - f.getX() * s.getZ(), f.getX() * s.getY() - f.getY() * s.getX());
}

inline Vec3 computeUnitVector(const Vec3& f)
{
	return f / f.computeMagnitude();
}
