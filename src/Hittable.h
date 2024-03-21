#ifndef HITTABLE_H
#define HITTABLE_H

#include "glm/glm.hpp"
#include "Ray.h"
#include <memory>

class Material;
class AABB;
struct HitRecord
{
    float t;
    glm::vec3 point;
    glm::vec3 normal;  // unit length, always points against the ray direction, use SetNormal()
    Material* material;
    glm::vec2 uv;


    bool frontFace;

    // outwardNormal is a unit length normal vector facing outwards of the surface
    inline void SetNormal(const Ray& r, const glm::vec3& outwardNormal)
    {
        frontFace = glm::dot(r.GetDir(), outwardNormal) < 0;
        normal    = glm::normalize(frontFace ? outwardNormal : -outwardNormal);
    }
};

class Hittable
{
public:
    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const = 0;
    virtual bool BoundingBox(AABB& outAABB) const                                      = 0;

    virtual float PDFValue(const glm::vec3& origin, const glm::vec3& direction) const
    {
        return 0;
    }
    virtual glm::vec3 Random(const glm::vec3& origin) const
    {
        return glm::vec3(1, 0, 0);
    }
};

#endif
