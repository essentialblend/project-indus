export module core_tests;

import std;
import vec3;
import fresneldielectric;
import dielectricbxdf;
import lambertianBxDF;

export auto testLambertianEnergyConservation() -> void 
{
  constexpr int nSamples{ 1'000'000 };
  const ColorRGB albedo{ 0.8, 0.2, 0.2 };
  LambertianBxDF lambert{ albedo };

  Vec3 wo{ 0, 0, 1 };
  std::mt19937 rng{ 42 };
  std::uniform_real_distribution<double> dist{ 0.0, 1.0 };

  ColorRGB estimate{ 0.0 };
  for (int i = 0; i < nSamples; ++i) 
  {
    Sample2D u{ dist(rng), dist(rng) };
    auto [wi, pdf, f, type] = lambert.sample(wo, u);
    if (pdf > 0) 
    {
      double cosTheta = wi[2];
      estimate += f * (cosTheta / pdf);
    }
  }
  estimate *= (1.0 / nSamples);
  std::println("Estimated reflectance = ({}, {}, {}), expected = ({}, {}, {})",
    estimate[0], estimate[1], estimate[2],
    albedo[0], albedo[1], albedo[2]);
}

export auto testDielectricEnergyConservation() -> void 
{
  const int nSamples = 1000000;
  const ColorRGB R{ 1.0 }, T{ 1.0 };
  double etaI = 1.0, etaT = 1.5;
  DielectricBxDF glass{ R, T, etaI, etaT };
  std::mt19937 rng{ 42 };
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  auto runAngle = [&](double cosTheta) {
    Vec3 wo{ std::sqrt(1.0 - cosTheta * cosTheta), 0, cosTheta };
    int reflect = 0, transmit = 0;
    ColorRGB sum{ 0 };
    for (int i = 0; i < nSamples; ++i) {
      Sample2D u{ dist(rng), dist(rng) };
      auto [wi, pdf, f, type] = glass.sample(wo, u);
      if ((type & BxDFType::Reflection) == BxDFType::Reflection) reflect++;
      if ((type & BxDFType::Transmission) == BxDFType::Transmission) transmit++;
      sum += f;
    }
    double refProb = double(reflect) / nSamples;
    double trProb = double(transmit) / nSamples;
    double Fr = FresnelDielectric{ etaI, etaT }.evaluate(cosTheta);
    std::println("cosTheta={} -> MC refl={}, trans={}, Fresnel={}, sum={} (~~1)",
      cosTheta, refProb, trProb, Fr, refProb + trProb);
    std::println("Avg f = ({}, {}, {})",
      sum[0] / nSamples, sum[1] / nSamples, sum[2] / nSamples);
    };

  runAngle(1.0); 
  runAngle(0.707); 
  runAngle(0.1);   
}