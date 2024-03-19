#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include "3DMath/Random.h"

class Camera
{
public:
    Camera(const glm::vec3& pos, const glm::vec3& lookAt, const glm::vec3& up, float vFOV, float aspectRatio, float aperture, float focusDist)
    {
        float viewPortHeight = 2 * tan(glm::radians(vFOV) / 2);
        float viewPortWidth  = aspectRatio * viewPortHeight;

        m_w = glm::normalize(pos - lookAt);
        m_u = glm::normalize(glm::cross(up, m_w));
        m_v = glm::normalize(glm::cross(m_w, m_u));

        m_origin     = pos;
        m_horizontal = focusDist * viewPortWidth * m_u;
        m_vertical   = focusDist * viewPortHeight * m_v;
        m_bottomLeft = m_origin - m_horizontal / 2.0f - m_vertical / 2.0f - focusDist * m_w;

        m_lensRadius = aperture / 2;
    }

    Ray GetRay(float u, float v) const
    {
        glm::vec2 rand   = m_lensRadius * math::RandomInUnitDisk<float>();
        glm::vec3 offset = m_u * rand.x + m_v * rand.y;

        return Ray(m_origin + offset, m_bottomLeft + u * m_horizontal + v * m_vertical - m_origin - offset);
    }

private:
    glm::vec3 m_origin;
    glm::vec3 m_horizontal;
    glm::vec3 m_vertical;
    glm::vec3 m_bottomLeft;
    glm::vec3 m_u, m_v, m_w;
    float m_lensRadius;
};

#endif
