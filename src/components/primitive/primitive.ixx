export module primitive;

import std;
import intersectionconstructs;
import shape;
import material;
import ray;
import types;
import surfaceinteraction;
import bounds;

export class Primitive
{
public:
  virtual ~Primitive() = default;

  virtual std::optional<ShapeIntersection> intersect(const Ray&) const = 0;

  virtual bool intersectP(const Ray&) const = 0;

  virtual Bounds3f getBounds() const noexcept = 0;
};

export class GeometricPrimitive final : public Primitive
{
public:
  GeometricPrimitive(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material) noexcept;
  std::optional<ShapeIntersection> intersect(const Ray&) const override;

  bool intersectP(const Ray&) const override;

  [[nodiscard]] virtual Bounds3f getBounds() const noexcept override;

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