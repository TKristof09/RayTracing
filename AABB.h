#ifndef AABB_H
#define AABB_H

#include "3DMath/3DMath.h"
#include "Ray.h"

class AABB
{
public:
	AABB() {}
	AABB(const math::Vec3d& min, const math::Vec3d& max)
	{
		m_bounds[0] = min;
		m_bounds[1] = max;
	}

	// optimization from https://www.researchgate.net/publication/220494140_An_Efficient_and_Robust_Ray-Box_Intersection_Algorithm
	bool Hit(const Ray& r, double tMin, double tMax) const
	{
		auto invDir = r.GetInvDir();
		for(int i = 0; i < 3; ++i)
		{

			double t0 = (m_bounds[r.GetSign(i)][i] - r.GetOrigin()[i]) * invDir[i];
			double t1 = (m_bounds[1 - r.GetSign(i)][i] - r.GetOrigin()[i]) * invDir[i];


			tMin = t0 > tMin ? t0 : tMin;
			tMax = t1 < tMax ? t1 : tMax;
			if(tMax <= tMin)
				return false;
		}
		return true;
	}

	friend AABB SurroundingBox(AABB box1, AABB box2);

	math::Vec3d GetMin() const { return m_bounds[0]; }
	math::Vec3d GetMax() const { return m_bounds[1]; }
private:
	math::Vec3d m_bounds[2];
};


AABB SurroundingBox(AABB box1, AABB box2)
{
	math::Vec3d min(fmin(box1.m_bounds[0].x, box2.m_bounds[0].x),
			fmin(box1.m_bounds[0].y, box2.m_bounds[0].y),
			fmin(box1.m_bounds[0].z, box2.m_bounds[0].z));

	math::Vec3d max(fmax(box1.m_bounds[1].x, box2.m_bounds[1].x),
			fmax(box1.m_bounds[1].y, box2.m_bounds[1].y),
			fmax(box1.m_bounds[1].z, box2.m_bounds[1].z));
	return AABB(min, max);
}
#endif
