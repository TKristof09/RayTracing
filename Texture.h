#ifndef TEXTURE_H
#define TEXTURE_H

#include "3DMath/3DMath.h"
#include <memory>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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


class ImageTexture : public Texture
{
public:
	ImageTexture() {}
	ImageTexture(const std::string& filename):
		m_channels(3)
	{
		m_data = stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, m_channels);
		if(!m_data)
		{
			std::cerr << "ERROR: Couldn't load texture image: " << filename << std::endl;
			m_width = m_height = 0;
		}
	}
	virtual math::Vec3d Sample(const math::Vec2d& uv, const math::Vec3d& point) const override
	{
		if(m_data == nullptr || m_width == 0 || m_height == 0)
			return math::Vec3d(1, 0, 1); // cyan

		// coords in [0, width] x [0, height]
		int x = static_cast<int>(math::clamp(uv.x, 0.0, 1.0) * m_width);;
		int y = static_cast<int>((1 - math::clamp(uv.y, 0.0, 1.0)) * m_height);; // flip

		if(x >= m_width)
			x = m_width - 1;
		if(y >= m_height)
			y = m_height - 1;

		double colorScale = 1.0 / 255.0;

		auto pixel = m_data + y * m_width * m_channels + x * m_channels;
		return math::Vec3d(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);




	}
private:
	unsigned char* m_data;
	int m_width, m_height;
	int m_channels;
};
#endif
