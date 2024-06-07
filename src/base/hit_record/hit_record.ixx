export module hit_record;

import <memory>;

import ray;
import vec3;

export class IMaterial;

export class HitRecord
{
public:
	PointOLD hitPoint{};
	Vec3 normalVec{};
	double root{};
	bool hitFrontFace{};
	std::shared_ptr<IMaterial> hitMaterial;

	void setFaceNormal(const Ray& inputRay, const Vec3& outwardNormal)
	{
		hitFrontFace = computeDot(inputRay.getDirection(), outwardNormal) < 0;
		normalVec = hitFrontFace ? Vec3(outwardNormal) : Vec3(-outwardNormal);
	}
};