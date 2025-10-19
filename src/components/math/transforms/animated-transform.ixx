export module animatedtransform;

import std;
import transform;
import types;
import concepts;
import ray;
import mathalgebra;

export class AnimatedTransform final
{
public:
  AnimatedTransform() noexcept = default;

  constexpr explicit AnimatedTransform(const Transform4f& startTransform, Float startTime, const Transform4f& endTransform, Float endTime) noexcept;

  constexpr Transform4f interpolate(Float t) const noexcept;

  constexpr Point3f operator()(Point3f p, Float t) const noexcept;
  constexpr Point3f applyInverse(Point3f p, Float t) const noexcept;

  Ray operator()(const Ray& r) const noexcept;
  Ray applyInverse(const Ray& r) const noexcept;
  
  template<FloatOnlyVector3Like V>
  constexpr V operator()(const V& v, Float t) const noexcept;

  template<FloatOnlyVector3Like V>
  constexpr V applyInverse(const V& v, Float t) const noexcept;

  constexpr bool hasScale() const noexcept;

  Float getStartTime() const noexcept;
  Float getEndTime() const noexcept;

private:
  Transform4f m_startTransform{}; Transform4f m_endTransform{};
  Float m_startTime{}; Float m_endTime{};
};

constexpr AnimatedTransform::AnimatedTransform(const Transform4f& startTransform, Float startTime,
  const Transform4f& endTransform, Float endTime) noexcept
  : m_startTransform{ startTransform }, m_endTransform{ endTransform }, m_startTime{ startTime }, m_endTime{ endTime } {}

constexpr Transform4f AnimatedTransform::interpolate(Float t) const noexcept
{
  // static for now. Proper interpolation TBD. CRITICAL!
  return m_startTransform;
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
  const Float t{ r.getTimeSeconds() };
  
  return Ray{ interpolate(t).applyInverse(r.getOrigin()), interpolate(t).applyInverse(r.getDirection()), r.getTimeSeconds() };
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
