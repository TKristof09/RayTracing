#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Hittable.h"
#include <memory>
#include <vector>
#include "AABB.h"

class HittableList : public Hittable
{
public:
	HittableList() {}

	void Add(std::shared_ptr<Hittable> obj)
	{
		m_objects.push_back(obj);
	}

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const override;
	virtual bool BoundingBox(AABB& outAABB) const override;

	auto& GetObjects() { return m_objects; }
private:
	std::vector<std::shared_ptr<Hittable>> m_objects;
};

bool HittableList::Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const
{
	HitRecord temp;
	bool hit = false;
	double closest = tMax;

	for(const auto& obj : m_objects)
	{
		if(obj->Hit(r, tMin, closest, temp))
		{
			hit = true;
			closest = temp.t;
			outRecord = temp;
		}
	}

	return hit;
}
bool HittableList::BoundingBox(AABB& outAABB) const
{
	if(m_objects.empty())
		return false;

	AABB temp;
	bool firstIter = true;
	for(const auto& obj : m_objects)
	{
		if(!obj->BoundingBox(temp))
			return false;
		outAABB = firstIter ? temp : SurroundingBox(temp, outAABB);
		firstIter = false;
	}
	return true;
}
#endif
