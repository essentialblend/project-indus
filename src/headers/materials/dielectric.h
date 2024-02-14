#pragma once


class MDielectric : public Material
{
public:
	MDielectric(const double refInd) : refractionIndex(refInd) {}

	bool handleRayScatter(const Ray& inputRay, Ray& scatteredRay, const HitRecord& hitRec, ColorVec3& colorAttenuation) const override
	{
		colorAttenuation = ColorVec3(1.F);
		double refractionRatio = hitRec.frontFace ? (1.f / refractionIndex) : refractionIndex;
		Vec3 unitDirectionVec = computeUnitVector(inputRay.getRayDirection());
		double cosineTheta{ std::fmin(computeDotProduct(-unitDirectionVec, hitRec.hitNormalVec), 1.f) };
		double sinTheta{ std::sqrt(1.f - (cosineTheta * cosineTheta)) };

		bool cannotRefract{ (refractionRatio * sinTheta) > 1.f };


		if (cannotRefract || computeSchlicksApproxForReflectance(cosineTheta, refractionRatio) > UGenRNGDouble())
		{
			scatteredRay = Ray(hitRec.hitPoint, computeReflectionDirection(unitDirectionVec, hitRec.hitNormalVec));
		}
		else
		{
			scatteredRay = Ray(hitRec.hitPoint, computeRefractionDirection(unitDirectionVec, hitRec.hitNormalVec, refractionRatio));
		}

		return true;
	}

private:
	double refractionIndex;

	static double computeSchlicksApproxForReflectance(double cosine, double refIDX)
	{
		double r0Coeff = (1 - refIDX) / (1 + refIDX);
		r0Coeff = r0Coeff * r0Coeff;
		return r0Coeff + ((1 - r0Coeff) * std::pow(1 - cosine, 5));
	}

};