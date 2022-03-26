#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include "3DMath/Random.h"

class Camera
{
public:
	Camera(const math::Vec3d& pos, const math::Vec3d& lookAt, const math::Vec3d& up, double vFOV, double aspectRatio, double aperture, double focusDist)
	{
		double viewPortHeight = 2 * tan(math::ToRadians(vFOV) / 2);
		double viewPortWidth = aspectRatio * viewPortHeight;

		m_w = math::normalize(pos - lookAt);
		m_u = math::normalize(math::cross(up, m_w));
		m_v = math::normalize(math::cross(m_w, m_u));

		m_origin = pos;
		m_horizontal = focusDist * viewPortWidth * m_u;
		m_vertical = focusDist * viewPortHeight * m_v;
		m_bottomLeft = m_origin - m_horizontal / 2.0 - m_vertical / 2.0 - focusDist * m_w;

		m_lensRadius = aperture / 2;
	}

	Ray GetRay(double u, double v)
	{
		math::Vec2d rand = m_lensRadius * math::RandomInUnitDisk<double>();
		math::Vec3d offset = m_u * rand.x + m_v * rand.y;

		return Ray(m_origin + offset, m_bottomLeft + u * m_horizontal + v * m_vertical - m_origin - offset);
	}
private:
	math::Vec3d m_origin;
	math::Vec3d m_horizontal;
	math::Vec3d m_vertical;
	math::Vec3d m_bottomLeft;
	math::Vec3d m_u, m_v, m_w;
	double m_lensRadius;
};

#endif
