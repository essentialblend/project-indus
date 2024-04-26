export module world_object;

import vec3;
import color;
import ray;
import interval;
import hit_record;

import <vector>;
import <memory>;

export class WorldObject abstract
{
public:
	explicit WorldObject() noexcept = default;
	virtual ~WorldObject() noexcept = default;
	
	virtual bool checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const = 0;
};

export class WorldObjectList : public WorldObject
{
public:
	explicit WorldObjectList() noexcept = default;
	explicit WorldObjectList(std::unique_ptr<WorldObject> worldObj) noexcept;

	void clearList() noexcept;
	void addWorldObj(std::unique_ptr<WorldObject> worldObj) noexcept;

	bool checkHit(const Ray& inputRay, Interval rayInterval, HitRecord& hitRec) const override;

private:
	std::vector<std::unique_ptr<WorldObject>> m_worldObjectList{};
};