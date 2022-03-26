#ifndef RAY_H
#define RAY_H

#include "3DMath/3DMath.h"

class Ray
{
public:
	Ray() {};
	Ray(const math::Vec3d& origin, const math::Vec3d& dir):
		m_origin(origin), m_dir(dir), m_invDir(1.0 / dir)
	{
		m_signs[0] = m_invDir.x < 0;
		m_signs[1] = m_invDir.y < 0;
		m_signs[2] = m_invDir.z < 0;
	}

	math::Vec3d At(double t) const
	{
		return m_origin + t * m_dir;
	}

	math::Vec3d GetOrigin() const { return m_origin; }
	math::Vec3d GetDir() const { return m_dir; }
	math::Vec3d GetInvDir() const { return m_invDir; }
	int GetSign(int i) const { return m_signs[i]; }

private:
	math::Vec3d m_origin;
	math::Vec3d m_dir;
	math::Vec3d m_invDir;
	int m_signs[3];
};

#endif
