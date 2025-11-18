export module indus.core.geom.util;

import std;

import indus.core.types;
import indus.core.math.fp.ii;
import indus.core.math.algebra.iv;

export
{
  // Pending deeper understanding
  [[nodiscard]] std::optional<Point2f> invertBilinear(const Point2f& p, const std::array<Point2f, 4>& v) noexcept
  {
    const Vec2f a{ v[0][0], v[0][1] };
    const Vec2f b{ v[1][0] - v[0][0], v[1][1] - v[0][1] };
    const Vec2f c{ v[2][0] - v[0][0], v[2][1] - v[0][1] };
    const Vec2f d{ v[0][0] - v[1][0] - v[2][0] + v[3][0], v[0][1] - v[1][1] - v[2][1] + v[3][1] };
    const Vec2f q{ p[0] - a[0], p[1] - a[1] };

    const Float E{ computeCross2(d, b) };
    const Float F{ computeCross2(c, d) };
    const Float G{ computeCross2(b, c) };
    const Float Hb{ computeCross2(q, b) };
    const Float Hc{ computeCross2(q, c) };

    const Float A{ G * F };
    const Float B{ Hb * F + G * G - E * Hc };
    const Float C{ Hb * G };

    Float vsol{};

    if (std::abs(A) < Float{ 1e-12 })
    {
      if (std::abs(B) < Float{ 1e-20 }) return std::nullopt;
      vsol = -C / B;
    }
    else
    {
      const auto roots{ evaluateQuadratic(A, B, C) };

      if (!roots) return std::nullopt;

      const auto& [r0, r1] = *roots;
      const Float r0c{ clamp(r0, Float{}, Float(1)) };
      const Float r1c{ clamp(r1, Float{}, Float(1)) };

      vsol = (std::abs(r0c - r0) <= std::abs(r1c - r1)) ? r0c : r1c;
    }

    const Float denom{ G + vsol * F };

    if (std::abs(denom) < Float{ 1e-20 }) return std::nullopt;

    const Float usol{ Hc / denom };

    return Point2f{ clamp(usol, Float{}, Float{ 1 }), clamp(vsol, Float{}, Float{ 1 }) };
  }

  [[nodiscard]] bool sameHemisphere(const Vec3f& a, const Vec3f& b, const Vec3f& n) noexcept
  {
    return computeDot(a, n) * computeDot(b, n) > Float{};
  }

  [[nodiscard]] bool sameHemisphereLocal(const Vec3f& a, const Vec3f& b) noexcept
  {
    return (a[2] * b[2]) > Float{};
  }

  [[nodiscard]] Point3f offsetRayOrigin(const Point3f& p, const Vec3f& pError, const Normal3f& n, const Vec3f& w)
  {
    const Normal3f na{ Normal3f{std::abs(n[0]), std::abs(n[1]), std::abs(n[2])} };
    const Float d{ computeDot(Vec3f{na[0], na[1], na[2]}, pError) };

    Vec3f off{ d * Vec3f{n[0], n[1], n[2]} };

    if (computeDot(w, n) < Float{}) off = -off;

    Point3f po{ p + off };

    bool bumped{};

    for (Int i{}; i < 3; ++i)
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
      for (Int i{}; i < 3; ++i)
      {
        if (n[i] > 0) po[i] = nextFloatUp(po[i]);
        else if (n[i] < 0) po[i] = nextFloatDown(po[i]);
      }
    }

    return po;
  }
}

