export module onb;

import types;
import normal;
import mathconstants;
import mathfp;
import mathalgebra;

export class OrthonormalBasis final
{
public:
  constexpr OrthonormalBasis() noexcept = default;
  constexpr OrthonormalBasis(const Normal3f&, const Vec3f&, const Vec3f&) noexcept;

  static OrthonormalBasis fromPBRT(const Normal3f&);
  // static OrthonormalBasis fromFrisvad(const Normal3f&);

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
  const auto epsilon{ kSafeNormalizeLen<Float> };

  Normal3f n{ geometricNormal[0], geometricNormal[1], geometricNormal[2] };
  n = normalizeSafe(n, epsilon);

  if(n[0] == 0 && n[1] == 0 && n[2] == 0)
  {
    return OrthonormalBasis{ Normal3f{0,0,1}, Vec3f{1,0,0}, Vec3f{0,1,0} };
  }

  Vec3f t{};
  if (std::abs(n[0]) > std::abs(n[1])) 
  {
    const Float denom{ std::sqrt(sqr(n[0]) + sqr(n[2])) + epsilon };
    t = Vec3f{ -n[2] / denom, Float{},  n[0] / denom };
  }
  else 
  {
    const Float denom{ std::sqrt(sqr(n[1]) + sqr(n[2])) + epsilon };
    t = Vec3f{ Float{},  n[2] / denom, -n[1] / denom };
  }

  Vec3f b{ computeCross(n, t) };
  t = normalizeSafe(t, epsilon);
  b = normalizeSafe(b, epsilon);

  return OrthonormalBasis{ n, t, b };
}

// Pending deeper understanding
//OrthonormalBasis OrthonormalBasis::fromFrisvad(const Normal3f& normal)
//{
//  Vec3f tangent{};
//  Vec3f bitangent{};
//
//  if (normal[2] < -0.9999999f) 
//  {
//    tangent = Vec3f{ 0, -1, 0 };
//    bitangent = Vec3f{ -1, 0, 0 };
//  }
//  else 
//  {
//    Float a{ Float(1.0) / (Float(1.0) + normal[2]) };
//    Float bb{ -normal[0] * normal[1] * a };
//    tangent = Vec3f(1.0f - normal[0] * normal[0] * a, bb, -normal[0]);
//    bitangent = Vec3f(bb, Float(1.0) - normal[1] * normal[1] * a, -normal[1]);
//  }
//
//  return OrthonormalBasis{ normal, tangent, bitangent};
//}

constexpr auto OrthonormalBasis::localToWorld(const Vec3f& v) const noexcept
{
  return (m_tangent * v[0]) + (m_bitangent * v[1]) + (m_normal * v[2]);
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



