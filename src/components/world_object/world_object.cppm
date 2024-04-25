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

bool WorldObjectList::checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const
{
	HitRecord tempHitRec;
	bool hitAnything = false;
	double closestSoFar = rayInterval.getMax();

	for (const auto& worldObj : m_worldObjectList)
	{
		if (worldObj->checkHit(inputRay, Interval(rayInterval.getMin(), closestSoFar), tempHitRec))
		{
			hitAnything = true;
			closestSoFar = tempHitRec.root;
			hitRec = tempHitRec;
		}
	}
	return hitAnything;
}