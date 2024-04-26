export module hit_record;

import <memory>;

import ray;
import vec3;

export class Material;

export class HitRecord
{
public:
	Point hitPoint{};
	Vec3 normalVec{};
	double root{};
	bool hitFrontFace{};
	std::shared_ptr<Material> hitMaterial;

	void setFaceNormal(const Ray& inputRay, const Vec3& outwardNormal)
	{
		hitFrontFace = computeDot(inputRay.getDirection(), outwardNormal) < 0;
		normalVec = hitFrontFace ? outwardNormal : -outwardNormal;
	}
};