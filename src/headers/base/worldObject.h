#pragma once

class HitRecord
{
public:
	PointVec3 hitPoint;
	Vec3 normalVec;
	double hitRoot;
	bool frontFace;

	void setFaceNormal(const Ray& inputRay, const Vec3& outwardNormal)
	{
		frontFace = computeDotProduct(inputRay.getRayDirection(), outwardNormal) < 0;
		normalVec = frontFace ? outwardNormal : -outwardNormal;
	}
};

class WorldObject
{
public:
	virtual ~WorldObject() = default;
	virtual bool checkHit(const Ray& inputRay, double rayTMin, double rayTMax, HitRecord& hitRec) const = 0;
};