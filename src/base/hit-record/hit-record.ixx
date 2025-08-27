export module hit_record;

import <memory>;

import ray;
import vec3;
import core_constructs;
import bsdf;

export class IMaterial;

export class HitRecord
{
public:
	Point hitPoint{};
	OrthonormalBasis shadingBasis{};
	std::unique_ptr<BSDF> surfaceBSDF{};

	double root{};
	bool hitFrontFace{};
	std::shared_ptr<IMaterial> hitMaterial;

	void setFaceNormal(const Ray& inputRay, const Vec3& unitOutwardNormal)
	{
		Vec3 localUnitNormal{ getUnit(unitOutwardNormal) };
		hitFrontFace = computeDot(inputRay.getDirection(), localUnitNormal) < 0;
		
		if (!hitFrontFace)
		{
			localUnitNormal = -localUnitNormal;
		}

		buildOrthonormalBasis(localUnitNormal, shadingBasis.m_unitTangentVec, shadingBasis.m_unitBitangentVec, shadingBasis.m_unitNormalVec);
	}
};