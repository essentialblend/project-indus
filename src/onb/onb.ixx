export module onb;

import types;
import normal;

export class OrthonormalBasis final
{
public:
  constexpr OrthonormalBasis() noexcept = default;
  constexpr OrthonormalBasis(const Normal3f&, const Vec3f&, const Vec3f&) noexcept;

  static OrthonormalBasis fromPBRT(const Normal3f&);
  static OrthonormalBasis fromFrisvad(const Normal3f&);

  [[nodiscard]] constexpr Normal3f getNormal() const noexcept;
  [[nodiscard]] constexpr Vec3f getTangent() const noexcept;
  [[nodiscard]] constexpr Vec3f getBitangent() const noexcept;

  constexpr auto localToWorld(const Vec3f&) const noexcept;
  constexpr auto worldToLocal(const Vec3f&) const noexcept;

private:
  Normal3f m_normal{};
  Vec3f m_tangent{};
  Vec3f m_bitangent{};
};

constexpr OrthonormalBasis::OrthonormalBasis(const Normal3f& n, const Vec3f& t, const Vec3f& b) noexcept : m_normal{ n }, m_tangent{ t }, m_bitangent{ b } {}

OrthonormalBasis OrthonormalBasis::fromPBRT(const Normal3f& geometricNormal) 
{
  Normal3f n{ geometricNormal[0], geometricNormal[1], geometricNormal[2] };
  n = normalize(n);

  Vec3f t{};
  if (std::abs(n[0]) > std::abs(n[1])) 
  {
    const Float denom{ std::sqrt(n[0] * n[0] + n[2] * n[2]) + Float(1e-30) };
    t = Vec3f{ -n[2] / denom, Float(0.0),  n[0] / denom };
  }
  else 
  {
    const Float denom{ std::sqrt(n[1] * n[1] + n[2] * n[2]) + Float(1e-30) };
    t = Vec3f{ Float(0.0),  n[2] / denom, -n[1] / denom };
  }

  Vec3f b{ computeCross(n, t) };
  t = normalize(t);
  b = normalize(b);

  return OrthonormalBasis{ n, t, b };
}

OrthonormalBasis OrthonormalBasis::fromFrisvad(const Normal3f& normal)
{
  Vec3f tangent{};
  Vec3f bitangent{};

  if (normal[2] < -0.9999999f) 
  {
    tangent = Vec3f{ 0, -1, 0 };
    bitangent = Vec3f{ -1, 0, 0 };
  }
  else 
  {
    Float a{ Float(1.0) / (Float(1.0) + normal[2]) };
    Float bb{ -normal[0] * normal[1] * a };
    tangent = Vec3f(1.0f - normal[0] * normal[0] * a, bb, -normal[0]);
    bitangent = Vec3f(bb, Float(1.0) - normal[1] * normal[1] * a, -normal[1]);
  }

  return OrthonormalBasis{ normal, tangent, bitangent};
}

constexpr auto OrthonormalBasis::localToWorld(const Vec3f& v) const noexcept
{
  return (m_tangent * v[0]) + (m_bitangent * v[1]) + (m_normal * v[2]);

  // Simpler direct version above, expansion below for semantic clarity on the affine transform
  /*Vec4f tangentVec{ m_tangent[0], m_tangent[1], m_tangent[2], 0 };
  Vec4f bitangentVec{ m_bitangent[0], m_bitangent[1], m_bitangent[2], 0 };
  Vec4f normalVec{ m_normal[0], m_normal[1], m_normal[2], 0 };
  Vec4f affineFinalCol{ 0, 0, 0, 1 };
  Vec4f v4{ v[0], v[1], v[2], 0 };

  Mat4f ONBBasisTransform{ tangentVec, bitangentVec, normalVec, affineFinalCol };

  Vec4f transformResult{ ONBBasisTransform * v4 };

  return Vec3f{ transformResult[0], transformResult[1], transformResult[2] };*/
}

constexpr auto OrthonormalBasis::worldToLocal(const Vec3f& v) const noexcept
{
  return Vec3f{ computeDot(v, m_tangent), computeDot(v, m_bitangent), computeDot(v, m_normal) };
}

constexpr Normal3f OrthonormalBasis::getNormal() const noexcept
{
  return m_normal;
}

constexpr Vec3f OrthonormalBasis::getTangent() const noexcept
{
  return m_tangent;
}

constexpr Vec3f OrthonormalBasis::getBitangent() const noexcept
{
  return m_bitangent;
}



