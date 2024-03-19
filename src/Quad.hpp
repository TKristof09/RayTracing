#pragma once 

#include "AABB.h"
#include "Hittable.h"

class Quad : public Hittable
{
public:
    Quad(math::Vec3d Q, math::Vec3d U, math::Vec3d V, std::shared_ptr<Material> mat) :
        m_Q(Q), m_U(U), m_V(V), m_material(mat)
    {
        m_normal = math::normalize(math::cross(m_U, m_V));
    }

    virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const override
    {
        double t = math::dot(m_Q - r.GetOrigin(), m_normal) / math::dot(r.GetDir(), m_normal);
        if (t < tMin || t > tMax)
            return false;

        math::Vec3d P = r.At(t);
        math::Vec3d QP = P - m_Q;
        double u = math::dot(QP, m_U);
        double v = math::dot(QP, m_V);
        if (u < 0 || u > 1 || v < 0 || v > 1)
            return false;

        outRecord.t = t;
        outRecord.point = P;
        outRecord.SetNormal(r, m_normal);
        outRecord.material = m_material;
        outRecord.uv = math::Vec2d(u, v);
        return true;
    }

    virtual bool BoundingBox(AABB& outAABB) const override
    {
        outAABB = AABB(m_Q, m_Q + m_U + m_V);
        outAABB.Pad();
        return true;
    }
private:
    math::Vec3d m_Q, m_U, m_V;
    math::Vec3d m_normal;
    std::shared_ptr<Material> m_material;
};
