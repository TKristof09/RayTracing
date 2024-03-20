#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm/glm.hpp"
#include "Allocator.hpp"

class Texture
{
public:
    virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& point) const = 0;
};


class SolidColor : public Texture
{
public:
    SolidColor() {}
    SolidColor(const glm::vec3& color) : m_color(color) {}
    SolidColor(float r, float g, float b) : m_color(glm::vec3(r, g, b)) {}

    virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& point) const override
    {
        return m_color;
    }

private:
    glm::vec3 m_color;
};

class CheckerTexture : public Texture
{
public:
    CheckerTexture() {}
    CheckerTexture(Texture* odd, Texture* even) : m_odd(odd), m_even(even) {}
    CheckerTexture(const glm::vec3& c1, const glm::vec3& c2) : m_odd(g_materialAllocator.Allocate<SolidColor>(c1)), m_even(g_materialAllocator.Allocate<SolidColor>(c2)) {}

    virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& point) const override
    {
        auto sines = sin(10 * point.x) * sin(10 * point.y) * sin(10 * point.z);
        if(sines < 0)
            return m_odd->Sample(uv, point);
        else
            return m_even->Sample(uv, point);
    }

private:
    Texture* m_odd;
    Texture* m_even;
};


class ImageTexture : public Texture
{
public:
    ImageTexture() {}
    ImageTexture(const std::string& filename) : m_channels(3)
    {
        m_data = stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, m_channels);
        if(!m_data)
        {
            std::cerr << "ERROR: Couldn't load texture image: " << filename << std::endl;
            m_width = m_height = 0;
        }
    }
    virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& point) const override
    {
        if(m_data == nullptr || m_width == 0 || m_height == 0)
            return glm::vec3(1, 0, 1);  // cyan

        // coords in [0, width] x [0, height]
        int x = static_cast<int>(glm::clamp(uv.x, 0.0f, 1.0f) * m_width);
        ;
        int y = static_cast<int>((1 - glm::clamp(uv.y, 0.0f, 1.0f)) * m_height);
        ;  // flip

        if(x >= m_width)
            x = m_width - 1;
        if(y >= m_height)
            y = m_height - 1;

        float colorScale = 1.0f / 255.0f;

        auto pixel = m_data + y * m_width * m_channels + x * m_channels;
        return glm::vec3(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }

private:
    unsigned char* m_data;
    int m_width, m_height;
    int m_channels;
};
#endif
