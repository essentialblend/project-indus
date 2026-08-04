export module indus.geom.list_aggregate;

import std;

import indus.core.geom.ray;
import indus.core.geom.bounds;

import indus.geom.primitive;
import indus.geom.intersection_constructs;
import indus.stats.accumulator;

export class ListAggregate final : public Primitive
{
public:

  explicit ListAggregate(std::vector<std::shared_ptr<Primitive>>) noexcept;

  [[nodiscard]] Bounds3f getBounds() const noexcept override;
  [[nodiscard]] std::optional<ShapeIntersection> intersect(const Ray&) const override;
  [[nodiscard]] bool intersectP(const Ray&) const override;
  [[nodiscard]] std::string toString() const noexcept override;

private:
  std::vector<std::shared_ptr<Primitive>> m_primitives{};
};

ListAggregate::ListAggregate(std::vector<std::shared_ptr<Primitive>> primitives) noexcept : m_primitives{ std::move(primitives) } {}

Bounds3f ListAggregate::getBounds() const noexcept 
{
  Bounds3f b{};
  for (const auto& p : m_primitives) 
    b = Bounds3f::getUnion(b, p->getBounds());
  
  return b;
}

std::optional<ShapeIntersection> ListAggregate::intersect(const Ray& ray) const 
{
  StatsAccumulator::recordRegularIntersectionTest();
  std::optional<ShapeIntersection> best{};
  UInt64 tests{};
  
  Ray r{ ray };

  for (const auto& p : m_primitives) 
  {
    ++tests;
    if (auto hit = p->intersect(r)) 
    {
      best = hit;
      r.setTMax(hit->tHit);
    }
  }

  StatsAccumulator::recordRayPrimitiveTests(tests);

  return best;
}

bool ListAggregate::intersectP(const Ray& ray) const 
{
  StatsAccumulator::recordRegularIntersectionTest();
  UInt64 tests{};

  for (const auto& p : m_primitives) 
  {
    ++tests;
    if (p->intersectP(ray))
    {
      StatsAccumulator::recordRayPrimitiveTests(tests);
      return true;
    }
  }

  StatsAccumulator::recordRayPrimitiveTests(tests);
  
  return false;
}

std::string ListAggregate::toString() const noexcept
{
  return "Primitive-List";
}
