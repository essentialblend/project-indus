export module indus.core.geom.quaternion;

import std;

import indus.core.types;
import indus.core.math.constants.i;
import indus.core.math.fp.ii;
import indus.core.math.trig.iii;
import indus.core.math.algebra.iv;

export class Quaternion final
{
public:
  constexpr Quaternion() noexcept = default;
  constexpr Quaternion(const Vec3f& vectorPart, Float scalarPart) noexcept;

  [[nodiscard]] constexpr const Vec3f& getVector() const noexcept;
  [[nodiscard]] constexpr Float getScalar() const noexcept;

  [[nodiscard]] Quaternion operator-() const noexcept;

  Quaternion& operator*=(const Quaternion& RHS) noexcept;
  Quaternion& operator*=(Float scalar) noexcept;
  Quaternion& operator/=(Float scalar) noexcept;
  Quaternion& operator+=(const Quaternion& RHS) noexcept;
  Quaternion& operator-=(const Quaternion& RHS) noexcept;

  [[nodiscard]] Quaternion operator*(Quaternion RHS) const noexcept;
  [[nodiscard]] Quaternion operator*(Float scalar) const noexcept;
  [[nodiscard]] Quaternion operator/(Float scalar) const noexcept;

  [[nodiscard]] std::string toString() const;

private:
  Vec3f m_quaternionVector{};
  Float m_quaternionScalar{ Float{1} };
};

export
{
  Quaternion operator*(Float scalar, Quaternion quaternion) noexcept;

  Float computeDot(const Quaternion& LHS, const Quaternion& RHS) noexcept;
  Float length(const Quaternion& quaternion) noexcept;
  Quaternion normalize(const Quaternion& quaternion) noexcept;

  Float angleBetween(const Quaternion& first, const Quaternion& second) noexcept;
  Quaternion slerp(Float t, Quaternion first, Quaternion second) noexcept;
}

constexpr Quaternion::Quaternion(const Vec3f& vectorPart, Float scalarPart) noexcept : m_quaternionVector{ vectorPart }, m_quaternionScalar{ scalarPart } {}

constexpr const Vec3f& Quaternion::getVector() const noexcept
{
  return m_quaternionVector;
}

constexpr Float Quaternion::getScalar() const noexcept
{
  return m_quaternionScalar;
}

Quaternion Quaternion::operator-() const noexcept
{
  Quaternion negated{ *this };

  negated.m_quaternionVector = -negated.m_quaternionVector;
  negated.m_quaternionScalar = -negated.m_quaternionScalar;

  return negated;
}

// Hamilton product: (v,w)*(u,s) = ( w*u + s*v + v×u , w*s - v·u )
Quaternion& Quaternion::operator*=(const Quaternion& RHS) noexcept
{
  const Vec3f LHSVector{ getVector() };
  const Float LHSScalar{ getScalar() };

  const Vec3f RHSVector{ RHS.getVector() };
  const Float RHSScalar{ RHS.getScalar() };

  const Vec3f vectorPart{ (RHSVector * LHSScalar) + (LHSVector * RHSScalar) + computeCross(LHSVector, RHSVector) };

  const Float vectorDot{ sumOfProducts(LHSVector[0], RHSVector[0], LHSVector[1], RHSVector[1], LHSVector[2], RHSVector[2]) };

  const Float scalarPart{ differenceOfProducts(LHSScalar, RHSScalar, vectorDot, Float{ 1 }) };

  m_quaternionVector = vectorPart;
  m_quaternionScalar = scalarPart;

  return *this;
}

Quaternion& Quaternion::operator*=(Float scalar) noexcept
{
  m_quaternionVector = getVector() * scalar;
  m_quaternionScalar = getScalar() * scalar;

  return *this;
}

Quaternion& Quaternion::operator/=(Float scalar) noexcept
{
  // Assert pending
  const Float inverseScalar{ Float{ 1 } / scalar };

  return (*this *= inverseScalar);
}

Quaternion& Quaternion::operator+=(const Quaternion& RHS) noexcept
{
  m_quaternionVector = getVector() + RHS.getVector();
  m_quaternionScalar = getScalar() + RHS.getScalar();

  return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& RHS) noexcept
{
  m_quaternionVector = getVector() - RHS.getVector();
  m_quaternionScalar = getScalar() - RHS.getScalar();

  return *this;
}

Quaternion Quaternion::operator*(Quaternion RHS) const noexcept
{
  Quaternion result{ *this };
  result *= RHS;

  return result;
}

Quaternion Quaternion::operator*(Float scalar) const noexcept
{
  Quaternion result{ *this };
  result *= scalar;

  return result;
}

Quaternion Quaternion::operator/(Float scalar) const noexcept
{
  Quaternion result{ *this };
  result /= scalar;

  return result;
}

std::string Quaternion::toString() const
{
  const Vec3f& vectorPart{ getVector() };

  return "Quaternion[w=" + std::to_string(getScalar()) +
    ", v=(" + std::to_string(vectorPart[0]) + "," +
    std::to_string(vectorPart[1]) + "," +
    std::to_string(vectorPart[2]) + ")]";
}

Quaternion operator*(Float scalar, Quaternion quaternion) noexcept
{
  return quaternion * scalar;
}

Float computeDot(const Quaternion& LHS, const Quaternion& RHS) noexcept
{
  const Vec3f& LHSVector{ LHS.getVector() };
  const Vec3f& RHSVector{ RHS.getVector() };

  const Float vectorDot{ sumOfProducts(LHSVector[0], RHSVector[0], LHSVector[1], RHSVector[1], LHSVector[2], RHSVector[2]) };

  return fusedMultiplyAdd(LHS.getScalar(), RHS.getScalar(), vectorDot);
}

Float length(const Quaternion& quaternion) noexcept
{
  return safeSqrt(computeDot(quaternion, quaternion));
}

Quaternion normalize(const Quaternion& quaternion) noexcept
{
  const Float quaternionLength{ length(quaternion) };
  if (quaternionLength <= Float{}) return quaternion;
  
  return quaternion / quaternionLength;
}

Float angleBetween(const Quaternion& first, const Quaternion& second) noexcept
{
  const Float dotValue{ computeDot(first, second) };

  if (dotValue < Float{})
  {
    Quaternion sum{ first };
    sum += second;

    return kPi_v<Float> - (Float{ 2 } * safeASin(length(sum) / Float{ 2 }));
  }

  Quaternion difference{ second };
  difference -= first;

  return Float{ 2 } * safeASin(length(difference) / Float{ 2 });
}

Quaternion slerp(Float t, Quaternion first, Quaternion second) noexcept
{
  const Float dotValue{ computeDot(first, second) };

  // Take the shortest arc.
  if (dotValue < Float{}) second = -second;

  const Float interpolationAngle{ angleBetween(first, second) };
  const Float sinThetaOverTheta{ sinexOverx(interpolationAngle) }; // ~1 when angle ~ 0

  const Float firstWeight{ (Float{1} - t) * (sinexOverx((Float{1} - t) * interpolationAngle) / sinThetaOverTheta) };

  const Float secondWeight{ t * (sinexOverx(t * interpolationAngle) / sinThetaOverTheta) };

  Quaternion result{ first * firstWeight };
  result += (second * secondWeight);
  
  return result;
}



