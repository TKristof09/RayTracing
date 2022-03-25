#ifndef RAY_H
#define RAY_H

#include "3DMath/3DMath.h"

class Ray
{
public:
	Ray() {};
	Ray(const math::Vec3d& origin, const math::Vec3d& dir):
		m_origin(origin), m_dir(dir) {}

	math::Vec3d At(double t) const
	{
		return m_origin + t * m_dir;
	}

	math::Vec3d GetOrigin() const { return m_origin; }
	math::Vec3d GetDir() const { return m_dir; }

private:
	math::Vec3d m_origin;
	math::Vec3d m_dir;
};

#endif
