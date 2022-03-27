#ifndef TEXTURE_H
#define TEXTURE_H

#include "3DMath/3DMath.h"
#include <memory>

class Texture
{
public:
	virtual math::Vec3d Sample(const math::Vec2d& uv, const math::Vec3d& point) const = 0;
};


class SolidColor : public Texture
{
public:
	SolidColor() {}
	SolidColor(const math::Vec3d color):
		m_color(color) {}
	SolidColor(double r, double g, double b):
		m_color(math::Vec3d(r,g,b)) {}

	virtual math::Vec3d Sample(const math::Vec2d& uv, const math::Vec3d& point) const override
	{
		return m_color;
	}
private:
	math::Vec3d m_color;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture() {}
	CheckerTexture(std::shared_ptr<Texture> odd, std::shared_ptr<Texture> even):
		m_odd(odd), m_even(even) {}
	CheckerTexture(const math::Vec3d& c1, const math::Vec3d& c2):
		m_odd(std::make_shared<SolidColor>(c1)), m_even(std::make_shared<SolidColor>(c2)) {}

	virtual math::Vec3d Sample(const math::Vec2d& uv, const math::Vec3d& point) const override
	{
		auto sines = sin(10 * point.x) * sin(10 * point.y) * sin(10 * point.z);
		if(sines < 0)
			return m_odd->Sample(uv, point);
		else
			return m_even->Sample(uv, point);
	}

private:
	std::shared_ptr<Texture> m_odd;
	std::shared_ptr<Texture> m_even;
};
#endif
