#ifndef BVH_H
#define BVH_H

#include <iostream>
#include "Hittable.h"
#include "HittableList.h"
#include "3DMath/Random.h"

class BVHNode : public Hittable
{
public:
	BVHNode();
	BVHNode( HittableList& list):
		BVHNode(list.GetObjects(), 0, list.GetObjects().size()) {}
	BVHNode( std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end);

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const override;
	virtual bool BoundingBox(AABB& outAABB) const override
	{
		outAABB = m_aabb;
		return true;
	}

private:
	std::shared_ptr<Hittable> m_left, m_right;
	AABB m_aabb;
};

bool BVHNode::Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const
{
	if(!m_aabb.Hit(r, tMin, tMax))
		return false;

	bool hitLeft = m_left->Hit(r, tMin, tMax, outRecord);
	bool hitRight = m_right->Hit(r, tMin, hitLeft ? outRecord.t : tMax, outRecord);

	return hitLeft || hitRight;
}

BVHNode::BVHNode( std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end)
{
	auto objectsModif = objects; // create a modifiable vector

	int axis = math::RandomInt(0, 2);

	auto comp = [axis](const std::shared_ptr<Hittable> lhs, const std::shared_ptr<Hittable> rhs)
				{
					AABB lhsBox;
					AABB rhsBox;
					if(!lhs->BoundingBox(lhsBox) || !rhs->BoundingBox(rhsBox))
						std::cerr << "BVHNode contains element with no bounding box" << std::endl;
					return lhsBox.GetMin()[axis] < rhsBox.GetMin()[axis];
				};

	size_t numObjects = end - start;
	if(numObjects == 1)
		m_left = m_right = objects[start];
	else if(numObjects == 2)
	{
		if(comp(objects[start], objects[start+1]))
		{
			m_left = objects[start];
			m_right = objects[start+1];
		}
		else
		{
			m_left = objects[start+1];
			m_right = objects[start];
		}
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comp);

		size_t mid = start + numObjects / 2;
		m_left = std::make_shared<BVHNode>(objects, start, mid);
		m_right = std::make_shared<BVHNode>(objects, mid, end);
	}

	AABB leftBox, rightBox;
	if(!m_left->BoundingBox(leftBox) || !m_right->BoundingBox(rightBox))
		std::cerr << "BVHNode contains element with no bounding box" << std::endl;

	m_aabb = SurroundingBox(leftBox, rightBox);
}

#endif
