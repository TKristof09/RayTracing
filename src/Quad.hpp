#pragma once

#include "AABB.h"
#include "Hittable.h"

class Quad : public Hittable
{
public:
    Quad(glm::vec3 Q, glm::vec3 U, glm::vec3 V, Material* mat) : m_Q(Q), m_U(U), m_V(V), m_material(mat)
    {
        glm::vec3 n = glm::cross(m_U, m_V);
        m_normal    = glm::normalize(n);
        m_W         = n / glm::dot(n, n);
    }

    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override
    {
        float nDotDir = glm::dot(m_normal, r.GetDir());
        if(glm::abs(nDotDir) < 0.0001f)
            return false;  // Ray is parallel to the plane

        float t = glm::dot(m_Q - r.GetOrigin(), m_normal) / nDotDir;
        if(t < tMin || t > tMax)
            return false;

        glm::vec3 P  = r.At(t);
        glm::vec3 QP = P - m_Q;
        float u      = glm::dot(m_W, glm::cross(QP, m_V));
        float v      = glm::dot(m_W, glm::cross(m_U, QP));
        if(u < 0 || u > 1 || v < 0 || v > 1)
            return false;

        outRecord.t        = t;
        outRecord.point    = P;
        outRecord.material = m_material;
        outRecord.uv       = glm::vec2(u, v);
        outRecord.SetNormal(r, m_normal);
        return true;
    }

    virtual bool BoundingBox(AABB& outAABB) const override
    {
        outAABB = AABB(m_Q, m_Q + m_U + m_V);
        outAABB.Pad();
        return true;
    }

private:
    glm::vec3 m_Q, m_U, m_V;
    glm::vec3 m_W;
    glm::vec3 m_normal;
    Material* m_material;
};
