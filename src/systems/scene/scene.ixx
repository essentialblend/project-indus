export module indus.scene;

import std;

import indus.core.geom.bounds;
import indus.core.geom.ray;

import indus.geom.primitive;
import indus.geom.intersection_constructs;


export class Scene final
{
public:
  explicit Scene(std::shared_ptr<Primitive>) noexcept;

  [[nodiscard]] Bounds3f getWorldBounds() const noexcept;

  [[nodiscard]] std::optional<ShapeIntersection> intersect(const Ray&) const;
  [[nodiscard]] bool intersectP(const Ray&) const;

  [[nodiscard]] const std::shared_ptr<Primitive>& getSceneRoot() const noexcept;

private:
  std::shared_ptr<Primitive> m_root;
};

Scene::Scene(std::shared_ptr<Primitive> root) noexcept : m_root{ std::move(root) } {}

Bounds3f Scene::getWorldBounds() const noexcept 
{
  return m_root ? m_root->getBounds() : Bounds3f{};
}

std::optional<ShapeIntersection> Scene::intersect(const Ray& r) const 
{
  return m_root ? m_root->intersect(r) : std::nullopt;
}

bool Scene::intersectP(const Ray& r) const 
{
  return m_root && m_root->intersectP(r);
}

const std::shared_ptr<Primitive>& Scene::getSceneRoot() const noexcept
{ 
  return m_root; 
}