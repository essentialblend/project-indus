#pragma once

class WorldObjectList : public WorldObject
{
public:
	std::vector<std::shared_ptr<WorldObject>> WOList;

	WorldObjectList() {}
	WorldObjectList(std::shared_ptr<WorldObject> wObj) { addToWorld(wObj); }

	void addToWorld(std::shared_ptr<WorldObject> wObj)
	{
		WOList.push_back(wObj);
	}
	void clearWorld() { WOList.clear(); };

	virtual bool checkHit(const Ray& inputRay, Interval validInterval, HitRecord& hitRec) const override
	{
		HitRecord tempRec;
		bool didItHit{ false };
		double closestHitPointYet = validInterval.getIntervalMaxRange();

		for (const std::shared_ptr<WorldObject>& WO : WOList)
		{
			if (WO->checkHit(inputRay, Interval(validInterval.getIntervalMinRange(), closestHitPointYet), tempRec))
			{
				didItHit = true;
				closestHitPointYet = tempRec.hitRoot;
				hitRec = tempRec;
			}
		}

		return didItHit;
	}

};