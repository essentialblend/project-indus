export module core_util;

import <type_traits>;
import <limits>;
import <numbers>;
import <random>;

export std::uint64_t gRngSeed{ 1337 };

export constexpr double UPi{ std::numbers::pi };
export constexpr double UInfinity{ std::numeric_limits<double>::infinity() };

export constexpr double UDegreesToRadians(double degreesValue)
{
	return degreesValue * UPi / 180.0;
}

export constexpr double URadiansToDegrees(double radiansValue)
{
	return radiansValue * 180.0 / UPi;
}

export template<typename ValueType>
ValueType UGenRNG(ValueType min = 0, ValueType max = 1)
{
  // Static thread counter to ensure a unique seed for each thread
  static std::atomic<std::uint64_t> threadCounter{ 0 };

  // Generate a unique seed
  static thread_local std::mt19937 engine
  {
    []{
      // Use a unique ID for each thread to generate a different seed
      const std::uint64_t id{ 1u + threadCounter.fetch_add(1, std::memory_order_relaxed) };

      // Space out the seeds used by each thread to avoid cross-pollination/collisions
      const std::uint64_t z{ gRngSeed + 0x9E37797F4A7C15ull * id };
      
      //
      return std::mt19937{static_cast<std::uint32_t>(z ^ (z >> 32))};
    }() 
  };
    
  if constexpr (std::is_integral_v<ValueType>)
  {
    std::uniform_int_distribution<ValueType> distr(min, max);
    return distr(engine);
  }
  else
  {
    std::uniform_real_distribution<ValueType> distr(min, max);
    return distr(engine);
  }
}

export double fresnelDielectric(double cosThetaI, double etaI, double etaT) 
{
  cosThetaI = std::clamp(cosThetaI, -1.0, 1.0);

  // Figure out if we are entering or exiting
  bool entering = cosThetaI > 0.0;
  if (!entering) {
    std::swap(etaI, etaT);
    cosThetaI = std::fabs(cosThetaI);
  }

  // Compute sinThetaT using Snell’s law
  double sinThetaI = std::sqrt(std::max(0.0, 1.0 - cosThetaI * cosThetaI));
  double sinThetaT = etaI / etaT * sinThetaI;

  // Total internal reflection
  if (sinThetaT >= 1.0) return 1.0;

  double cosThetaT = std::sqrt(std::max(0.0, 1.0 - sinThetaT * sinThetaT));

  double rParl = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
    ((etaT * cosThetaI) + (etaI * cosThetaT));
  double rPerp = ((etaI * cosThetaI) - (etaT * cosThetaT)) /
    ((etaI * cosThetaI) + (etaT * cosThetaT));

  return 0.5 * (rParl * rParl + rPerp * rPerp);
}

export auto evaluateQuadratic(double a, double b, double c) -> std::optional<std::pair<double, double>>
{
  double discr{ (b * b) - (4.0 * a * c) };
  if (discr < 0.0) return std::nullopt;

  double sqrtDiscr{ std::sqrt(discr) };

  double q{ (b < 0) ? -0.5 * (b - sqrtDiscr) : -0.5 * (b + sqrtDiscr) };

  double t0{ q / a };
  double t1{ c / q };

  if (t0 > t1)
  {
    std::swap(t0, t1);
  }

  return std::make_pair(t0, t1);
}

