export module indus.core.geom.animatedtransform;

import std;

import indus.core.types;
import indus.core.concepts;
import indus.core.geom.transform;
import indus.core.geom.ray;
import indus.core.geom.quaternion;
import indus.core.geom.util;
import indus.core.geom.bounds;
import indus.core.math.fp.ii;
import indus.core.math.algebra.iv;


export class AnimatedTransform final
{
public:
  AnimatedTransform() noexcept;

  explicit AnimatedTransform(const Transform4f& startTransform, Float startTime, const Transform4f& endTransform, Float endTime) noexcept;

  Transform4f interpolate(Float time) const noexcept;

  constexpr Point3f operator()(Point3f p, Float t) const noexcept;
  constexpr Point3f applyInverse(Point3f p, Float t) const noexcept;

  Ray operator()(const Ray& r) const noexcept;
  Ray applyInverse(const Ray& r) const noexcept;
  
  template<FloatOnlyVector3Like V>
  constexpr V operator()(const V& v, Float t) const noexcept;

  template<FloatOnlyVector3Like V>
  constexpr V applyInverse(const V& v, Float t) const noexcept;

  constexpr bool hasScale() const noexcept;

  [[nodiscard]] Float getStartTime() const noexcept;
  [[nodiscard]] Float getEndTime() const noexcept;

  [[nodiscard]] Bounds3f motionBounds(const Bounds3f& primitiveBounds) const noexcept;

private:
  Transform4f m_startTransform{}; Transform4f m_endTransform{};
  Float m_startTime{}; Float m_endTime{};
  DecomposedTRS m_startDecomposed{};
  DecomposedTRS m_endDecomposed{};
};

AnimatedTransform::AnimatedTransform() noexcept
  : m_startTransform{}
  , m_endTransform{}
  , m_startTime{}
  , m_endTime{}
  , m_startDecomposed{ m_startTransform.decomposeTRS() }
  , m_endDecomposed{ m_endTransform.decomposeTRS() }
{
}

AnimatedTransform::AnimatedTransform(const Transform4f& startTransform, Float startTime,
  const Transform4f& endTransform, Float endTime) noexcept
  : m_startTransform{ startTransform }
  , m_endTransform{ endTransform }
  , m_startTime{ startTime }
  , m_endTime{ endTime }
  , m_startDecomposed{ m_startTransform.decomposeTRS() }
  , m_endDecomposed{ m_endTransform.decomposeTRS() }
{
}

Transform4f AnimatedTransform::interpolate(Float time) const noexcept
{
  if (time <= m_startTime || m_startTime == m_endTime) return m_startTransform;
  if (time >= m_endTime) return m_endTransform;

  const Float normalizedTime{ (time - m_startTime) / (m_endTime - m_startTime) };

  const auto& startDecomposed{ m_startDecomposed };
  const auto& endDecomposed{ m_endDecomposed };

  const Vec3f interpolatedTranslation{
    lerp(normalizedTime, startDecomposed.translation[0], endDecomposed.translation[0]),
    lerp(normalizedTime, startDecomposed.translation[1], endDecomposed.translation[1]),
    lerp(normalizedTime, startDecomposed.translation[2], endDecomposed.translation[2])
  };

  const Quaternion interpolatedRotation{ slerp(normalizedTime, startDecomposed.rotation, endDecomposed.rotation) };

  Mat3f interpolatedScale{ Mat3f::zero() };
  for (Idx rowIndex{}; rowIndex < 3; ++rowIndex)
    for (Idx colIndex{}; colIndex < 3; ++colIndex)
      interpolatedScale[rowIndex, colIndex] =
      lerp(normalizedTime, startDecomposed.scale[rowIndex, colIndex], endDecomposed.scale[rowIndex, colIndex]);

  const Transform4f translationTransform{ Transform4f::translate(interpolatedTranslation) };
  const Transform4f rotationTransform{ makeRotationTransform(interpolatedRotation) };
  const Transform4f scaleTransform{ makeLinearTransform(interpolatedScale) };

  return translationTransform * rotationTransform * scaleTransform;
}

constexpr Point3f AnimatedTransform::operator()(Point3f p, Float t) const noexcept
{
  return interpolate(t)(p);
}

constexpr Point3f AnimatedTransform::applyInverse(Point3f p, Float t) const noexcept
{
  return interpolate(t).applyInverse(p);
}

Ray AnimatedTransform::operator()(const Ray& r) const noexcept
{ 
  return interpolate(r.getTimeSeconds())(r);
}

Ray AnimatedTransform::applyInverse(const Ray& r) const noexcept
{
  return interpolate(r.getTimeSeconds()).applyInverse(r);
}

template<FloatOnlyVector3Like V>
constexpr V AnimatedTransform::operator()(const V& v, Float t) const noexcept 
{
  return interpolate(t)(v);
}

template<FloatOnlyVector3Like V>
constexpr V AnimatedTransform::applyInverse(const V& v, Float t) const noexcept 
{
  return interpolate(t).applyInverse(v);
}

constexpr bool AnimatedTransform::hasScale() const noexcept 
{
  return m_startTransform.hasScale() || m_endTransform.hasScale();
}

Float AnimatedTransform::getStartTime() const noexcept
{
  return m_startTime;
}

Float AnimatedTransform::getEndTime() const noexcept
{
  return m_endTime;
}

// Conservative, but efficient for simple linear motion at the cost of some bloat. PBRT's algorithm is more accurate but complex. I'm parking it for now till we have scenes with objects undergoing rotations and/or non-uniform scaling.
Bounds3f AnimatedTransform::motionBounds(const Bounds3f& primitiveBounds) const noexcept
{
  const Mat4f& startMatrix{ m_startTransform.get() };
  const Mat4f& endMatrix{ m_endTransform.get() };
  bool linearMatches{ true };

  for (int row{}; row < 3 && linearMatches; ++row)
    for (int col{}; col < 3; ++col)
      if (startMatrix[row, col] != endMatrix[row, col])
      {
        linearMatches = false;
        break;
      }

  if (linearMatches)
  {
    const Bounds3f startBounds{ m_startTransform(primitiveBounds) };
    const Bounds3f endBounds{ m_endTransform(primitiveBounds) };

    return Bounds3f::getUnion(startBounds, endBounds);
  }

  const auto& startDecomposed{ m_startDecomposed };
  const auto& endDecomposed{ m_endDecomposed };

  Float maximumDistanceFromOrigin{};

  for (int cornerIndex{}; cornerIndex < 8; ++cornerIndex)
  {
    const Point3f cornerPoint{ primitiveBounds.getCorner(cornerIndex) };
    const Float cornerDistance{ euclideanLength(Vec3f{ cornerPoint[0], cornerPoint[1], cornerPoint[2] }) };
    maximumDistanceFromOrigin = std::max(maximumDistanceFromOrigin, cornerDistance);
  }

  const auto scaleFrobeniusSquaredAt = [&](Float normalizedTime) noexcept
  {
    Float frobeniusSquared{};
    for (Idx rowIndex{}; rowIndex < 3; ++rowIndex)
      for (Idx colIndex{}; colIndex < 3; ++colIndex)
      {
        const Float interpolatedElement{ lerp(normalizedTime, startDecomposed.scale[rowIndex, colIndex], endDecomposed.scale[rowIndex, colIndex]) };
        frobeniusSquared += interpolatedElement * interpolatedElement;
      }
      
    return frobeniusSquared;
    };

  Float maximumScaleFrobeniusSquared{ std::max(scaleFrobeniusSquaredAt(Float{ 0 }), scaleFrobeniusSquaredAt(Float{ 1 })) };

  const Mat3f scaleDelta{ endDecomposed.scale - startDecomposed.scale };

  Float quadraticCoefficientA{};
  Float quadraticCoefficientB{};

  for (Idx rowIndex{}; rowIndex < 3; ++rowIndex)
    for (Idx colIndex{}; colIndex < 3; ++colIndex)
    {
      const Float deltaElement{ scaleDelta[rowIndex, colIndex] };
      const Float startElement{ startDecomposed.scale[rowIndex, colIndex] };

      quadraticCoefficientA += deltaElement * deltaElement;
      quadraticCoefficientB += Float{ 2 } * startElement * deltaElement;
    }

  if (quadraticCoefficientA > Float{})
  {
    const Float extremumTime{ clamp(-quadraticCoefficientB / (Float{ 2 } * quadraticCoefficientA), Float{ 0 }, Float{ 1 }) };
    maximumScaleFrobeniusSquared = std::max(maximumScaleFrobeniusSquared, scaleFrobeniusSquaredAt(extremumTime));
  }

  const Float maximumRadius{ safeSqrt(maximumScaleFrobeniusSquared) * maximumDistanceFromOrigin };

  const Point3f startTranslationPoint{
    startDecomposed.translation[0], startDecomposed.translation[1], startDecomposed.translation[2]
  };

  const Point3f endTranslationPoint{
    endDecomposed.translation[0], endDecomposed.translation[1], endDecomposed.translation[2]
  };

  Bounds3f translationSegmentBounds{ startTranslationPoint };

  translationSegmentBounds = Bounds3f::getUnion(translationSegmentBounds, endTranslationPoint);

  return Bounds3f::expandBoundsByDelta(translationSegmentBounds, maximumRadius);
}
