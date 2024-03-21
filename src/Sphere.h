#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "AABB.h"
#include "Hittable.h"
#include "ONB.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "3DMath/Random.h"

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(const glm::vec3& center, float radius, Material* material) : m_center(center), m_radius(radius), m_material(material) {}

    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override;
    virtual bool BoundingBox(AABB& outAABB) const override;

    virtual float PDFValue(const glm::vec3& origin, const glm::vec3& direction) const override
    {
        HitRecord rec;
        if(!Hit(Ray(origin, direction), 0.001f, std::numeric_limits<float>::max(), rec))
            return 0;

        float cosThetaMax = sqrt(1 - m_radius * m_radius / glm::length2(m_center - origin));
        float solidAngle  = 2 * glm::pi<float>() * (1 - cosThetaMax);

        return 1 / solidAngle;
    }
    virtual glm::vec3 Random(const glm::vec3& origin) const override
    {
        glm::vec3 direction   = m_center - origin;
        float distanceSquared = glm::length2(direction);
        ONB uvw(direction);

        return uvw.Local(RandomToSphere(m_radius, distanceSquared));
    }

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

    static glm::vec3 RandomToSphere(float radius, float distanceSquared)
    {
        float r1 = math::RandomReal<float>();
        float r2 = math::RandomReal<float>();
        float z  = 1 + r2 * (sqrt(1 - radius * radius / distanceSquared) - 1);

        float phi = 2 * glm::pi<float>() * r1;
        float sq  = sqrt(1 - z * z);
        float x   = cos(phi) * sq;
        float y   = sin(phi) * sq;

        return glm::vec3(x, y, z);
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
