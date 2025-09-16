export module core_util;

import std;
import types;

export Float fresnelDielectric(Float cosThetaI, Float etaI, Float etaT) 
{
  cosThetaI = std::clamp(cosThetaI, Float(- 1.0), Float(1.0));

  // Figure out if we are entering or exiting
  bool entering = cosThetaI > 0.0;
  
  if (!entering) 
  {
    std::swap(etaI, etaT);
    cosThetaI = std::fabs(cosThetaI);
  }

  // Compute sinThetaT using Snell’s law
  Float sinThetaI = std::sqrt(std::max(Float(0.0), Float(1.0) - cosThetaI * cosThetaI));
  Float sinThetaT = etaI / etaT * sinThetaI;

  // Total internal reflection
  if (sinThetaT >= 1.0) return 1.0;

  Float cosThetaT = std::sqrt(std::max(Float(0.0), Float(1.0) - sinThetaT * sinThetaT));

  Float rParl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
  Float rPerp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));

  return Float(0.5) * (rParl * rParl + rPerp * rPerp);
}

export constexpr std::optional<std::pair<Float, Float>> evaluateQuadratic(Float a, Float b, Float c)
{
  Float discr{ (b * b) - (Float(4.0) * a * c) };
  if (discr < 0.0) return std::nullopt;

  Float sqrtDiscr{ std::sqrt(discr) };

  Float q{ (b < 0) ? Float(- 0.5) * (b - sqrtDiscr) : Float(- 0.5)* (b + sqrtDiscr)};

  Float t0{ q / a };
  Float t1{ c / q };

  if (t0 > t1)
  {
    std::swap(t0, t1);
  }

  return std::make_pair(t0, t1);
}
