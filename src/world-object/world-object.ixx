export module world_object;

import std;

import ray;
import hit_record;
import types;

export class WorldObject abstract
{
public:
	explicit WorldObject() noexcept = default;
	virtual ~WorldObject() noexcept = default;
	
	virtual bool checkHit(const Ray&, Float, HitRecord&) const = 0;
};

export class WorldObjectList : public WorldObject
{
public:
	explicit WorldObjectList() noexcept = default;
	explicit WorldObjectList(std::unique_ptr<WorldObject>) noexcept;

	void clearList() noexcept;
	void addWorldObj(std::unique_ptr<WorldObject>) noexcept;

	bool checkHit(const Ray&, Float, HitRecord&) const override;

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

bool WorldObjectList::checkHit(const Ray& incidentRay, Float tMax, HitRecord& hitRec) const
{
	HitRecord tempHitRec;
	bool hitAnything = false;
	Float closestSoFar = tMax;

	for (const auto& worldObj : m_worldObjectList)
	{
		if (worldObj->checkHit(incidentRay, closestSoFar, tempHitRec))
		{
			hitAnything = true;
			closestSoFar = tempHitRec.root;
			hitRec = std::move(tempHitRec);
		}
	}
	return hitAnything;
}