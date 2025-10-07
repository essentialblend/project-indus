export module world_object;

import std;

import ray;
import hit_record;
import types;
import surfaceinteraction;

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
	explicit WorldObjectList(std::unique_ptr<WorldObject>) noexcept;

	void clearList() noexcept;
	void addWorldObj(std::unique_ptr<WorldObject>) noexcept;

	std::optional<SurfaceInteraction> checkHit(const Ray&, Float) const override;

private:
	std::vector<std::unique_ptr<WorldObject>> m_worldObjectList{};
};

WorldObjectList::WorldObjectList(std::unique_ptr<WorldObject> worldObj) noexcept
{
	addWorldObj(std::move(worldObj));
}

void WorldObjectList::clearList() noexcept
{
	m_worldObjectList.clear();
}

void WorldObjectList::addWorldObj(std::unique_ptr<WorldObject> worldObj) noexcept
{
	m_worldObjectList.push_back(std::move(worldObj));
}

std::optional<SurfaceInteraction> WorldObjectList::checkHit(const Ray& incidentRay, Float tMax) const
{
	std::optional<SurfaceInteraction> best{};
	Float closest{ tMax };
	for (const auto& obj : m_worldObjectList)
	{
		if (auto surfaceInteraction{ obj->checkHit(incidentRay, closest) })
		{
			closest = surfaceInteraction->getTHit();
			best = std::move(surfaceInteraction);
		}
	}
	return best;
}