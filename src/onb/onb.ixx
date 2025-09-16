export module onb;

import types;
import normal;

export class OrthonormalBasis final
{
public:
  constexpr OrthonormalBasis() noexcept = default;
  constexpr OrthonormalBasis(const Vec3f&, const Vec3f&, const Vec3f&) noexcept;

  static OrthonormalBasis fromPBRT(const Normal3f&);
  static OrthonormalBasis fromFrisvad(const Normal3f&);

  [[nodiscard]] constexpr Normal3f getNormal() const noexcept;
  [[nodiscard]] constexpr Vec3f getTangent() const noexcept;
  [[nodiscard]] constexpr Vec3f getBitangent() const noexcept;

  constexpr auto localToWorld(const Vec3f&) const noexcept;
  constexpr auto worldToLocal(const Vec3f&) const noexcept;

private:
  Vec3f m_normal{};
  Vec3f m_tangent{};
  Vec3f m_bitangent{};
};

constexpr OrthonormalBasis::OrthonormalBasis(const Vec3f& n, const Vec3f& t, const Vec3f& b) noexcept : m_normal{ n }, m_tangent{ t }, m_bitangent{ b } {}

OrthonormalBasis OrthonormalBasis::fromPBRT(const Normal3f& normal) 
{
  Vec3f n{ normal[0], normal[1], normal[2] };
  n = normalize(n);

  Vec3f t{};
  if (std::abs(n[0]) > std::abs(n[1])) {
    const Float denom = std::sqrt(n[0] * n[0] + n[2] * n[2]) + Float(1e-30);
    const Float inv = Float(1.0) / denom;
    t = Vec3f{ -n[2] * inv, Float(0.0),  n[0] * inv };
  }
  else {
    const Float denom = std::sqrt(n[1] * n[1] + n[2] * n[2]) + Float(1e-30);
    const Float inv = Float(1.0) / denom;
    t = Vec3f{ Float(0.0),  n[2] * inv, -n[1] * inv };
  }

  Vec3f b = computeCross(n, t);
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

  return OrthonormalBasis{ Vec3f{normal[0], normal[1], normal[2]}, tangent, bitangent};
}

constexpr auto OrthonormalBasis::localToWorld(const Vec3f& v) const noexcept
{
  const Vec3f vNormal{ m_normal[0], m_normal[1], m_normal[2] };
  return m_tangent * v[0] + m_bitangent * v[1] + vNormal * v[2];
}

constexpr auto OrthonormalBasis::worldToLocal(const Vec3f& v) const noexcept
{
  const Vec3f vNormal{ m_normal[0], m_normal[1], m_normal[2] };
  return Vec3f{ computeDot(v, m_tangent), computeDot(v, m_bitangent), computeDot(v, vNormal) };
}

constexpr Normal3f OrthonormalBasis::getNormal() const noexcept
{
  return Normal3f{ m_normal };
}

constexpr Vec3f OrthonormalBasis::getTangent() const noexcept
{
  return m_tangent;
}

constexpr Vec3f OrthonormalBasis::getBitangent() const noexcept
{
  return m_bitangent;
}



