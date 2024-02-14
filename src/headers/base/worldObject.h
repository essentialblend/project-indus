#pragma once

class Material;

class HitRecord
{
public:
	PointVec3 hitPoint;
	Vec3 hitNormalVec;
	double hitRoot{ 0 };
	bool frontFace{ false };
	std::shared_ptr<Material> hitMaterial;

	void setFaceNormal(const Ray& inputRay, const Vec3& outwardNormal)
	{
		frontFace = computeDotProduct(inputRay.getRayDirection(), outwardNormal) < 0;
		hitNormalVec = frontFace ? outwardNormal : -outwardNormal;
	}
};

class WorldObject
{
public:
	virtual ~WorldObject() = default;
	virtual bool checkHit(const Ray& inputRay, Interval validInterval, HitRecord& hitRec) const = 0;
};