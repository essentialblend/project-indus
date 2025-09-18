export module core_util;

import std;
import types;
import ray;

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

export [[nodiscard]] Float nextFloatUp(Float x) noexcept
{
  return std::nextafter(x, std::numeric_limits<Float>::infinity());
}

export [[nodiscard]] Float nextFloatDown(Float x) noexcept
{
  return std::nextafter(x, -std::numeric_limits<Float>::infinity());
}

export [[nodiscard]] Point3f offsetRayOrigin(const Point3f& p, const Vec3f& pError, const Normal3f& n, const Vec3f& w) 
{
  const Normal3f na{ Normal3f{std::abs(n[0]), std::abs(n[1]), std::abs(n[2])} };
  const Float d{ computeDot(Vec3f{na[0],na[1],na[2]}, pError) };
  
  Vec3f off{ d * Vec3f{n[0],n[1],n[2]} };
  
  if (computeDot(w, n) < Float(0)) off = -off;
  
  Point3f po{ p + off };
  
  bool bumped{};
  
  for (int i{}; i < 3; ++i)
  { 
    if (off[i] > 0) 
    { 
      po[i] = nextFloatUp(po[i]); 
      bumped = true; 
    } 
    else if (off[i] < 0) 
    { 
      po[i] = nextFloatDown(po[i]); 
      bumped = true; 
    } 
  }
  
  if (!bumped) 
  { 
    for (int i{}; i < 3; ++i)
    { 
      if (n[i] > 0) po[i] = nextFloatUp(po[i]); 
      else if (n[i] < 0) po[i] = nextFloatDown(po[i]); 
    } 
  }
  
  return po;
}

export [[nodiscard]] constexpr Float gamma(std::int32_t n) noexcept 
{
  const Float eps = std::numeric_limits<Float>::epsilon();
  return (n * eps) / (Float(1) - (n * eps));
}