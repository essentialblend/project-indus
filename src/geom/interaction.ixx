export module indus.geom.interaction;

import std;

import indus.core.types;
import indus.core.math.fp.ii;
import indus.core.math.algebra.iv;
import indus.core.geom.ray;
import indus.core.geom.util;

export class Interaction
{
public:
  Interaction() noexcept = default;
  Interaction(const Point3f& worldPos, Float timeSec, const Vec3f& worldError, std::optional<Normal3f> worldNormal = std::nullopt) noexcept;

  Ray spawnRay(const Vec3f& worldDir) const;
  Ray spawnRayTo(const Point3f& worldTargetPoint) const;

  Point3f getWorldPosition() const noexcept;
  Float getTimeSeconds() const noexcept;
  const Vec3f& getWorldError() const noexcept;
  std::optional<Normal3f> tryGetWorldNormal() const noexcept;
  bool hasNormal() const noexcept;

private:
  Point3f m_worldPos{};
  Float m_timeSec{};
  Normal3f m_worldNormal{};
  Vec3f m_worldError{};
  bool m_hasNormal{ false };
};

Interaction::Interaction(const Point3f& worldPos, Float timeSec, const Vec3f& worldError, std::optional<Normal3f> worldNormal) noexcept : m_worldPos{ worldPos }, m_timeSec{ timeSec }, m_worldError{ worldError }
{
  if (worldNormal) { m_worldNormal = *worldNormal; m_hasNormal = true; }
}

Point3f Interaction::getWorldPosition() const noexcept
{
  return m_worldPos;
}

Float Interaction::getTimeSeconds() const noexcept
{
	return m_timeSec;
}

const Vec3f& Interaction::getWorldError() const noexcept
{
  return m_worldError;
}

std::optional<Normal3f> Interaction::tryGetWorldNormal() const noexcept
{
  return m_hasNormal ? std::optional{ m_worldNormal } : std::nullopt;
}

bool Interaction::hasNormal() const noexcept
{
	return m_hasNormal;
}

Ray Interaction::spawnRay(const Vec3f& unitWorldDir) const
{
  if (m_hasNormal)
  {
    Point3f origin{ offsetRayOrigin(m_worldPos, m_worldError, m_worldNormal, unitWorldDir) };
    return Ray{ origin, unitWorldDir, m_timeSec };
  }

  return Ray{ m_worldPos, unitWorldDir, m_timeSec };
}

// Handling tMax for lights/shadow rays TBD
Ray Interaction::spawnRayTo(const Point3f& worldTargetPoint) const
{
  Vec3f dir{ worldTargetPoint - m_worldPos };

  if (isZero(euclideanLengthSq(dir))) return Ray{ m_worldPos, Vec3f{}, m_timeSec };
  
  dir = normalize(dir);
  
  return spawnRay(dir);
}