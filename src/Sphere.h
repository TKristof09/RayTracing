#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "AABB.h"
#include "Hittable.h"
#include "glm/ext/scalar_constants.hpp"

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(const glm::vec3& center, float radius, Material* material) : m_center(center), m_radius(radius), m_material(material) {}

    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override;
    virtual bool BoundingBox(AABB& outAABB) const override;

private:
    glm::vec3 m_center;
    float m_radius;
    Material* m_material;

    static glm::vec2 GetUV(const glm::vec3& point)
    {
        float theta = acos(-point.y);
        float phi   = atan2(-point.z, point.x) + glm::pi<float>();

        return glm::vec2(phi / (2 * glm::pi<float>()), theta / glm::pi<float>());
    }
};

bool Sphere::Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const
{
    glm::vec3 oc = r.GetOrigin() - m_center;
    float a      = glm::length2(r.GetDir());
    float half_b = glm::dot(r.GetDir(), oc);  // we can simplify the 2 in the return value calculation
    float c      = glm::length2(oc) - m_radius * m_radius;

    float delta = half_b * half_b - a * c;  // actually delta / 4 but doesn't matter

    if(delta < 0.0)
        return false;
    float sqrtDelta = sqrt(delta);
    float root      = (-half_b - sqrtDelta) / a;

    if(root < tMin || root > tMax)
    {
        root = (-half_b + sqrtDelta) / a;
        if(root < tMin || root > tMax)
            return false;
    }

    outRecord.t      = root;
    outRecord.point  = r.At(outRecord.t);
    glm::vec3 normal = (outRecord.point - m_center) / m_radius;
    outRecord.SetNormal(r, normal);
    outRecord.material = m_material;
    outRecord.uv       = Sphere::GetUV(normal);
    return true;
}
bool Sphere::BoundingBox(AABB& outAABB) const
{
    outAABB = AABB(m_center - glm::vec3(m_radius), m_center + glm::vec3(m_radius));
    return true;
}


#endif
