export module indus.geom.surface_interaction;

import std;

import indus.core.types;
import indus.core.math.algebra.iv;
import indus.core.geom.onb;

import indus.geom.interaction;

import indus.shading.bsdf;
import indus.shading.material;

export class SurfaceInteraction : public Interaction
{
public:
  SurfaceInteraction() noexcept = default;
  SurfaceInteraction(const Point3f& worldPos, Float timeSec, Float rayParam, const Vec3f& worldError, const Normal3f& worldGeometricNormal) noexcept;

  Float getTHit() const noexcept;
  Normal3f getWorldGeometricNormal() const noexcept;
  const OrthonormalBasis& getShadingBasis() const noexcept;
  const Point2f& getUV() const noexcept;
  std::shared_ptr<Material> getMaterial() const noexcept;

  void setUV(const Point2f& uv) noexcept;
  void setShadingBasis(const OrthonormalBasis& basis) noexcept;
  void setMaterial(std::shared_ptr<Material> material) noexcept;
  void orientToIncident(const Ray& incidentRay) noexcept;

private:
  Float m_tHit{};
  Normal3f m_worldGeometricNormal{};
  OrthonormalBasis m_shadingBasis{};
  Point2f m_uv{};
  std::shared_ptr<Material> m_material{};
  bool m_frontFace{};
};

SurfaceInteraction::SurfaceInteraction(const Point3f& worldPos, Float timeSec, Float tHit, const Vec3f& worldError, const Normal3f& worldGeometricNormal) noexcept : Interaction{ worldPos, timeSec, worldError, worldGeometricNormal }, m_tHit{ tHit }, m_worldGeometricNormal{ worldGeometricNormal }, m_shadingBasis{ OrthonormalBasis::fromPBRT(worldGeometricNormal) } {}

Float SurfaceInteraction::getTHit() const noexcept
{
  return m_tHit;
}

Normal3f SurfaceInteraction::getWorldGeometricNormal() const noexcept
{
  return m_worldGeometricNormal;
}

const OrthonormalBasis& SurfaceInteraction::getShadingBasis() const noexcept
{
  return m_shadingBasis;
}

const Point2f& SurfaceInteraction::getUV() const noexcept
{
  return m_uv;
}

std::shared_ptr<Material> SurfaceInteraction::getMaterial() const noexcept
{
  return m_material;
}

void SurfaceInteraction::setUV(const Point2f& uv) noexcept 
{ 
  m_uv = uv; 
}

void SurfaceInteraction::setShadingBasis(const OrthonormalBasis& shadingBasis) noexcept 
{ 
  m_shadingBasis = shadingBasis;
}

void SurfaceInteraction::setMaterial(std::shared_ptr<Material> material) noexcept 
{ 
  m_material = std::move(material); 
}

void SurfaceInteraction::orientToIncident(const Ray& incident) noexcept
{
  m_frontFace = (computeDot(incident.getDirection(), Vec3f{ m_worldGeometricNormal[0], m_worldGeometricNormal[1],m_worldGeometricNormal[2] }) < 0);
}