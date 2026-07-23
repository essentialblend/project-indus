export module indus.geom.primitive;

import std;

import indus.core.types;
import indus.core.geom.bounds;
import indus.core.geom.ray;
import indus.core.geom.transform;
import indus.core.geom.animatedtransform;

import indus.geom.surfaceinteraction;
import indus.geom.intersection_constructs;
import indus.geom.shape;

import indus.geom.bridgetransforminteraction;


import indus.shading.material;

export class Primitive
{
public:
  virtual ~Primitive() = default;

  virtual std::optional<ShapeIntersection> intersect(const Ray&) const = 0;

  virtual bool intersectP(const Ray&) const = 0;

  virtual Bounds3f getBounds() const noexcept = 0;

  [[nodiscard]] virtual std::string toString() const noexcept = 0;
};

export class GeometricPrimitive final : public Primitive
{
public:
  GeometricPrimitive(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material) noexcept;
  std::optional<ShapeIntersection> intersect(const Ray&) const override;

  bool intersectP(const Ray&) const override;

  [[nodiscard]] virtual Bounds3f getBounds() const noexcept override;

  [[nodiscard]] virtual std::string toString() const noexcept override;

private:
  std::shared_ptr<Shape> m_shape{};
  std::shared_ptr<Material> m_material{};
};

GeometricPrimitive::GeometricPrimitive(std::shared_ptr<Shape> s,
  std::shared_ptr<Material> m) noexcept : m_shape{ std::move(s) }, m_material{ std::move(m) } {}

std::optional<ShapeIntersection> GeometricPrimitive::intersect(const Ray& ray) const
{
  auto shapeIntersection{ m_shape->intersect(ray) };

  if (!shapeIntersection) return std::nullopt;

  shapeIntersection->interaction.setMaterial(m_material);
  
  return shapeIntersection;
}

bool GeometricPrimitive::intersectP(const Ray& ray) const
{
  return m_shape->intersectP(ray);
}

Bounds3f GeometricPrimitive::getBounds() const noexcept
{
  return m_shape->getBounds();
}

std::string GeometricPrimitive::toString() const noexcept
{
  return "GeometricPrimitive";
}

export class TransformedPrimitive final : public Primitive
{
public:
  TransformedPrimitive(std::shared_ptr<Primitive> wrappedPrimitive, const Transform4f& renderFromPrimitive) noexcept;

  std::optional<ShapeIntersection> intersect(const Ray& ray) const override;
  bool intersectP(const Ray& ray) const override;
  Bounds3f getBounds() const noexcept override;

  [[nodiscard]] std::string toString() const noexcept override;

private:
  std::shared_ptr<Primitive> m_wrappedPrimitive{};
  Transform4f m_renderFromPrimitive{};
  Transform4f m_primitiveFromRender{};
};

TransformedPrimitive::TransformedPrimitive(std::shared_ptr<Primitive> wrappedPrimitive, const Transform4f& renderFromPrimitive) noexcept : m_wrappedPrimitive{ std::move(wrappedPrimitive) }, m_renderFromPrimitive{ renderFromPrimitive }, m_primitiveFromRender{ renderFromPrimitive.getInverseTransform() } {}

std::optional<ShapeIntersection> TransformedPrimitive::intersect(const Ray& ray) const
{
  const Ray primitiveSpaceRay{ m_renderFromPrimitive.applyInverse(ray) };

  auto primitiveSpaceIntersection{ m_wrappedPrimitive->intersect(primitiveSpaceRay) };
  if (!primitiveSpaceIntersection) return std::nullopt;

  primitiveSpaceIntersection->interaction = applyTransformToSurfaceInteraction(m_renderFromPrimitive, primitiveSpaceIntersection->interaction);

  return primitiveSpaceIntersection;
}

bool TransformedPrimitive::intersectP(const Ray& ray) const
{
  const Ray primitiveSpaceRay{ m_renderFromPrimitive.applyInverse(ray) };
  return m_wrappedPrimitive->intersectP(primitiveSpaceRay);
}

Bounds3f TransformedPrimitive::getBounds() const noexcept
{
  Bounds3f transformedBounds{};
  const Bounds3f wrappedBounds{ m_wrappedPrimitive->getBounds() };

  for (int cornerIndex{}; cornerIndex < 8; ++cornerIndex)
  {
    transformedBounds = Bounds3f::getUnion(transformedBounds, m_renderFromPrimitive(wrappedBounds.getCorner(cornerIndex)));
  }

  return transformedBounds;
}

std::string TransformedPrimitive::toString() const noexcept
{
  return "TransformedPrimitive(" + m_wrappedPrimitive->toString() + ")";
}

export class AnimatedPrimitive final : public Primitive
{
public:
  AnimatedPrimitive(std::shared_ptr<Primitive> wrappedPrimitive, const AnimatedTransform& renderFromPrimitive) noexcept;

  std::optional<ShapeIntersection> intersect(const Ray& ray) const override;
  bool intersectP(const Ray& ray) const override;
  Bounds3f getBounds() const noexcept override;

  [[nodiscard]] std::string toString() const noexcept override;

private:
  std::shared_ptr<Primitive> m_wrappedPrimitive{};
  AnimatedTransform m_renderFromPrimitive{};
  Bounds3f m_motionBounds{};
};

AnimatedPrimitive::AnimatedPrimitive(std::shared_ptr<Primitive> wrappedPrimitive, const AnimatedTransform& renderFromPrimitive) noexcept : m_wrappedPrimitive{ std::move(wrappedPrimitive) }
  , m_renderFromPrimitive{ renderFromPrimitive }, m_motionBounds{ renderFromPrimitive.motionBounds(m_wrappedPrimitive->getBounds()) } {}

std::optional<ShapeIntersection> AnimatedPrimitive::intersect(const Ray& ray) const
{
  const Transform4f renderFromPrimitiveAtTime{ m_renderFromPrimitive.interpolate(ray.getTimeSeconds()) };
  const Ray primitiveSpaceRay{ renderFromPrimitiveAtTime.applyInverse(ray) };

  auto primitiveSpaceIntersection{ m_wrappedPrimitive->intersect(primitiveSpaceRay) };
  if (!primitiveSpaceIntersection) return std::nullopt;

  primitiveSpaceIntersection->interaction =
    applyTransformToSurfaceInteraction(renderFromPrimitiveAtTime, primitiveSpaceIntersection->interaction);

  return primitiveSpaceIntersection;
}

bool AnimatedPrimitive::intersectP(const Ray& ray) const
{
  const Transform4f renderFromPrimitiveAtTime{ m_renderFromPrimitive.interpolate(ray.getTimeSeconds()) };
  const Ray primitiveSpaceRay{ renderFromPrimitiveAtTime.applyInverse(ray) };
  return m_wrappedPrimitive->intersectP(primitiveSpaceRay);
}

Bounds3f AnimatedPrimitive::getBounds() const noexcept
{
  return m_motionBounds;
}

std::string AnimatedPrimitive::toString() const noexcept
{
  return "AnimatedPrimitive(" + m_wrappedPrimitive->toString() + ")";
}
