#ifndef HITTABLE_H
#define HITTABLE_H

#include "3DMath/3DMath.h"
#include "Ray.h"

struct HitRecord
{
	double t;
	math::Vec3d point;
	math::Vec3d normal; // unit length
};

class Hittable
{
public:
	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const = 0;
};

#endif

