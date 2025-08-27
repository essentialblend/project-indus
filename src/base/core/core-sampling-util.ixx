export module core_sampling_util;

import vec3;
import std;
import core_util;

export
{
  [[nodiscard]] Vec3 genRandomVec(double min = 0, double max = 1)
  {
    return Vec3(UGenRNG(min, max), UGenRNG(min, max), UGenRNG(min, max));
  }

  //[[nodiscard]] Vec3 genRandomUnitDiskVec()
  //{
  //  while (true)
  //  {
  //    const auto randomVec = Vec3(UGenRNG<double>(-1, 1), UGenRNG<double>(-1, 1), 0);
  //    if (randomVec.getMagnitudeSq() < 1)
  //      return Vec3(randomVec);
  //  }
  //}

  [[nodiscard]] Vec3 concentricSampleDisk(double u1, double u2)
  {
    double sX{ (2 * u1) - 1 };
    double sY{ (2 * u2) - 1 };

    if (sX == 0 && sY == 0) return Vec3(0, 0, 0);

    double r{}; double theta{};

    if (std::abs(sX) > std::abs(sY))
    {
      r = sX;
      theta = (UPi / 4.0) * (sY / sX);
    }
    else
    {
      r = sY;
      theta = (UPi / 2.0) - ((UPi / 4.0) * (sX / sY));
    }

    return Vec3{ r * std::cos(theta), r * std::sin(theta), 0.0 };
  }


  [[nodiscard]] Vec3 genCosineWeightedHemisphereVec(double uniDistRNGForRadiusPolar, double uniDistRNGForThetaPolar)
  {
    Vec3 d = concentricSampleDisk(uniDistRNGForRadiusPolar, uniDistRNGForThetaPolar);

    // Lift onto hemisphere with cosine weighted distribution
    double z{ std::sqrt(std::max(0.0, 1.0 - d[0] * d[0] - d[1] * d[1])) };

    return Vec3{ d[0], d[1], z };
  }
}