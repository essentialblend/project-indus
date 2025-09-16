export module hit_record;

import <memory>;

import ray;
import vector;
import bsdf;
import point;
import onb;
import types;
import core_sampling_util;

import <cassert>;

export class IMaterial;

export class HitRecord
{
public:
	Point3f hitPoint{};
	OrthonormalBasis shadingBasis{};
	std::unique_ptr<BSDF> surfaceBSDF{};

	Float root{};
	bool hitFrontFace{};
	std::shared_ptr<IMaterial> hitMaterial;

	void setFaceNormal(const Ray& inputRay, const Normal3f& unitOutward)
	{
		Normal3f n{ unitOutward };
		hitFrontFace = computeDot(inputRay.getDirection(), Vec3f{ unitOutward[0], unitOutward[1], unitOutward[2] }) < 0;
		
		if (!hitFrontFace) n = Normal3f{ -n };

		shadingBasis = OrthonormalBasis::fromPBRT(n);
	}
};