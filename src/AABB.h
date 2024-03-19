#ifndef AABB_H
#define AABB_H

#include "Ray.h"

class AABB
{
public:
    AABB() {}
    AABB(const glm::vec3& min, const glm::vec3& max)
    {
        m_bounds[0] = min;
        m_bounds[1] = max;
    }

    // optimization from https://www.researchgate.net/publication/220494140_An_Efficient_and_Robust_Ray-Box_Intersection_Algorithm
    bool Hit(const Ray& r, float tMin, float tMax) const
    {
        auto invDir = r.GetInvDir();
        for(int i = 0; i < 3; ++i)
        {
            float t0 = (m_bounds[r.GetSign(i)][i] - r.GetOrigin()[i]) * invDir[i];
            float t1 = (m_bounds[1 - r.GetSign(i)][i] - r.GetOrigin()[i]) * invDir[i];


            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if(tMax <= tMin)
                return false;
        }
        return true;
    }

    friend AABB SurroundingBox(AABB box1, AABB box2);

    glm::vec3 GetMin() const { return m_bounds[0]; }
    glm::vec3 GetMax() const { return m_bounds[1]; }

    void Pad()
    {
        float padding = 0.0001;
        if(m_bounds[1].x - m_bounds[0].x < padding)
        {
            m_bounds[0].x -= padding;
            m_bounds[1].x += padding;
        }
        if(m_bounds[1].y - m_bounds[0].y < padding)
        {
            m_bounds[0].y -= padding;
            m_bounds[1].y += padding;
        }
        if(m_bounds[1].z - m_bounds[0].z < padding)
        {
            m_bounds[0].z -= padding;
            m_bounds[1].z += padding;
        }
    }

private:
    glm::vec3 m_bounds[2];
};


inline AABB SurroundingBox(AABB box1, AABB box2)
{
    glm::vec3 min = glm::min(box1.m_bounds[0], box2.m_bounds[0]);
    glm::vec3 max = glm::max(box1.m_bounds[1], box2.m_bounds[1]);
    return AABB(min, max);
}
#endif
