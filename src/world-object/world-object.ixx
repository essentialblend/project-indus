export module world_object;

import std;

import ray;
import hit_record;
import types;
import surfaceinteraction;
import primitive;
import constructs;
import shape;

export class WorldObject abstract
{
public:
	explicit WorldObject() noexcept = default;
	virtual ~WorldObject() noexcept = default;
	
	virtual std::optional<SurfaceInteraction> checkHit(const Ray&, Float) const = 0;
};

export class WorldObjectList : public WorldObject
{
public:
	explicit WorldObjectList() noexcept = default;
	explicit WorldObjectList(std::unique_ptr<GeometricPrimitive>) noexcept;

	void clearList() noexcept;
	void addWorldObj(std::unique_ptr<GeometricPrimitive>) noexcept;

	std::optional<SurfaceInteraction> checkHit(const Ray&, Float) const override;

	void setFastTOnly(bool b) noexcept;

private:
	std::vector<std::unique_ptr<GeometricPrimitive>> m_worldObjectList{};
	bool m_fastTOnly{ true };
};

WorldObjectList::WorldObjectList(std::unique_ptr<GeometricPrimitive> worldObj) noexcept
{
	addWorldObj(std::move(worldObj));
}

void WorldObjectList::clearList() noexcept
{
	m_worldObjectList.clear();
}

void WorldObjectList::addWorldObj(std::unique_ptr<GeometricPrimitive> worldObj) noexcept
{
	m_worldObjectList.push_back(std::move(worldObj));
}

std::optional<SurfaceInteraction> WorldObjectList::checkHit(const Ray& incidentRay, Float tMax) const
{
  // Full SI path
  if (!m_fastTOnly) {
    std::optional<SurfaceInteraction> best;          
    Float closest = tMax;                             
    for (const auto& prim : m_worldObjectList) {
      if (auto si = prim->intersect(incidentRay, closest)) {
        closest = si->tHit;                           
        best = std::move(si->interaction);            
      }
    }
    return best;
  }

  // T-only fast path: pick winner, then build one SI
  bool anyHit = false;
  Float closest = tMax;
  std::optional<QuadricIntersection> bestQ;
  size_t bestIdx = 0;

  for (size_t i = 0; i < m_worldObjectList.size(); ++i) {
    if (auto q = m_worldObjectList[i]->intersectT(incidentRay, closest)) {
      anyHit = true;
      closest = q->tHit;
      bestQ = std::move(q);
      bestIdx = i;
    }
  }
  if (!anyHit) return std::nullopt;
  return m_worldObjectList[bestIdx]->makeSurface(*bestQ, incidentRay);
}

void WorldObjectList::setFastTOnly(bool b) noexcept
{
	m_fastTOnly = b;
}
