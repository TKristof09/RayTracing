#ifndef HITTABLE_H
#define HITTABLE_H

#include "3DMath/3DMath.h"
#include "Ray.h"
#include <memory>

class Material;
class AABB;
struct HitRecord
{
	double t;
	math::Vec3d point;
	math::Vec3d normal; // unit length, always points against the ray direction, use SetNormal()
	std::shared_ptr<Material> material;
	math::Vec2d uv;


	bool frontFace;

	// outwardNormal is a unit length normal vector facing outwards of the surface
	inline void SetNormal(const Ray& r, const math::Vec3d& outwardNormal)
	{
		frontFace = math::dot(r.GetDir(), outwardNormal) < 0;
		normal = math::normalize(frontFace ? outwardNormal : -outwardNormal);
	}
};

class Hittable
{
public:
	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const = 0;
	virtual bool BoundingBox(AABB& outAABB) const = 0;
};

#endif

