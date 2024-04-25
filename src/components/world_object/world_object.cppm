import world_object;

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

bool WorldObjectList::checkHit(const Ray& inputRay, double rayT_min, double rayT_max, HitRecord& hitRec) const
{
	HitRecord tempHitRec;
	bool hitAnything = false;
	double closestSoFar = rayT_max;

	for (const auto& worldObj : m_worldObjectList)
	{
		if (worldObj->checkHit(inputRay, rayT_min, closestSoFar, tempHitRec))
		{
			hitAnything = true;
			closestSoFar = tempHitRec.root;
			hitRec = tempHitRec;
		}
	}

	return hitAnything;
}