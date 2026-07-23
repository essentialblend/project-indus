export module indus.core.geom.util;

import std;

import indus.core.types;
import indus.core.math.fp.ii;
import indus.core.math.algebra.iv;

import indus.core.geom.transform;
import indus.core.geom.quaternion;
import indus.core.geom.squarematrix;

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

  [[nodiscard]] inline Mat3f rotationMatrixFromQuaternion(Quaternion q) noexcept
  {
    q = normalize(q);

    const Float w{ q.getScalar() };
    const Vec3f v{ q.getVector() };
    const Float x{ v[0] }, y{ v[1] }, z{ v[2] };

    const Float xx{ x * x }, yy{ y * y }, zz{ z * z };
    const Float xy{ x * y }, xz{ x * z }, yz{ y * z };
    const Float wx{ w * x }, wy{ w * y }, wz{ w * z };

    Mat3f R{ Mat3f::identity() };

    R[0, 0] = 1 - 2 * (yy + zz);
    R[0, 1] = 2 * (xy - wz);
    R[0, 2] = 2 * (xz + wy);

    R[1, 0] = 2 * (xy + wz);
    R[1, 1] = 1 - 2 * (xx + zz);
    R[1, 2] = 2 * (yz - wx);

    R[2, 0] = 2 * (xz - wy);
    R[2, 1] = 2 * (yz + wx);
    R[2, 2] = 1 - 2 * (xx + yy);

    return R;
  }


  [[nodiscard]] inline Transform4f makeLinearTransform(const Mat3f& linear3x3)
  {
    Mat4f forward{ Mat4f::identity() };

    for (Idx r{}; r < 3; ++r) 
      for (Idx c{}; c < 3; ++c) 
        forward[r, c] = linear3x3[r, c];

    const Mat3f inv3{ linear3x3.inverse() };

    Mat4f inverse4{ Mat4f::identity() };

    for (Idx r{}; r < 3; ++r)
      for (Idx c{}; c < 3; ++c)
        inverse4[r, c] = inv3[r, c];

    return Transform4f{ forward, inverse4 };
  }

  [[nodiscard]] inline Transform4f makeRotationTransform(const Quaternion& q)
  {
    const Mat3f R{ rotationMatrixFromQuaternion(q) };
    const Mat3f Rt{ R.transpose() };
    
    Mat4f forward{ Mat4f::identity() };
    Mat4f inverse{ Mat4f::identity() };
    
    for (Idx r{}; r < 3; ++r) for (Idx c{}; c < 3; ++c)
    {
      forward[r, c] = R[r, c];
      inverse[r, c] = Rt[r, c];
    }

    return Transform4f{ forward, inverse };
  }


  [[nodiscard]] Bounds3f transformBounds(const Transform4f& transformToApply, const Bounds3f& inputBounds) noexcept
  {
    Bounds3f transformedBounds{};

    for (int cornerIndex{}; cornerIndex < 8; ++cornerIndex)
    {
      const Point3f cornerPoint{ inputBounds.getCorner(cornerIndex) };
      const Point3f transformedCorner{ transformToApply(cornerPoint) };
      transformedBounds = Bounds3f::getUnion(transformedBounds, transformedCorner);
    }

    return transformedBounds;
  }
}

