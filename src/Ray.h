#ifndef RAY_H
#define RAY_H

#include "glm/glm.hpp"

class Ray
{
public:
    Ray(){};
    Ray(const glm::vec3& origin, const glm::vec3& dir) : m_origin(origin), m_dir(glm::normalize(dir))
    {
        m_invDir = 1.0f / m_dir;

        m_signs[0] = m_invDir.x < 0;
        m_signs[1] = m_invDir.y < 0;
        m_signs[2] = m_invDir.z < 0;
    }

    glm::vec3 At(float t) const
    {
        return m_origin + t * m_dir;
    }

    glm::vec3 GetOrigin() const { return m_origin; }
    glm::vec3 GetDir() const { return m_dir; }
    glm::vec3 GetInvDir() const { return m_invDir; }
    int GetSign(int i) const { return m_signs[i]; }

private:
    glm::vec3 m_origin;
    glm::vec3 m_dir;
    glm::vec3 m_invDir;
    int m_signs[3];
};

#endif
