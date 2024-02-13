#pragma once

class Ray
{
public:
	Ray() {}

	Ray(const PointVec3& rayO, const Vec3& rayD) : rayOrigin(rayO), rayDirection(rayD) {}

	PointVec3 getRayOrigin() const
	{
		return rayOrigin;
	}

	PointVec3 getRayDirection() const
	{
		return rayDirection;
	}

	PointVec3 getPointOnRayAt(double t) const
	{
		return rayOrigin + (t * rayDirection);
	}


private:
	PointVec3 rayOrigin;
	Vec3 rayDirection;
};