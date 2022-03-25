#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"


class Camera
{
public:
	Camera()
	{

		const double aspectRatio = 16.0 / 9.0;
		double viewPortHeight = 2.0;
		double viewPortWidth = aspectRatio * viewPortHeight;
		double focalLength = 1.0; // distance between camera and projection plane

		m_origin = math::Vec3d(0,0,0);
		m_horizontal = math::Vec3d(viewPortWidth, 0, 0);
		m_vertical = math::Vec3d(0, viewPortHeight, 0);
		m_bottomLeft = m_origin - m_horizontal / 2.0 - m_vertical / 2.0 - math::Vec3d(0, 0, focalLength);
	}

	Ray GetRay(double u, double v)
	{
		return Ray(m_origin, m_bottomLeft + u * m_horizontal + v * m_vertical - m_origin);
	}
private:
	math::Vec3d m_origin;
	math::Vec3d m_horizontal;
	math::Vec3d m_vertical;
	math::Vec3d m_bottomLeft;

};

#endif
