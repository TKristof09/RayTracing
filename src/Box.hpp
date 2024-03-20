#ifndef BOX_H
#define BOX_H

#include "AABB.h"
#include "Hittable.h"
#include "HittableList.h"
#include <vector>
#include "Allocator.hpp"
#include "Quad.hpp"

class Box : public Hittable
{
public:
    Box(glm::vec3 min, glm::vec3 max, Material* mat) : m_min(min), m_max(max), m_material(mat)
    {
        glm::vec3 dx = glm::vec3(max.x - min.x, 0, 0);
        glm::vec3 dy = glm::vec3(0, max.y - min.y, 0);
        glm::vec3 dz = glm::vec3(0, 0, max.z - min.z);

        m_list.Add(g_shapeAllocator.Allocate<Quad>(min, dz, dy, mat));                              // left
        m_list.Add(g_shapeAllocator.Allocate<Quad>(min, dx, dz, mat));                              // bottom
        m_list.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(min.x, max.y, max.z), dx, -dz, mat));  // top
        m_list.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(max.x, min.y, max.z), -dz, dy, mat));  // right
        m_list.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(max.x, min.y, min.z), -dx, dy, mat));  // back
        m_list.Add(g_shapeAllocator.Allocate<Quad>(glm::vec3(min.x, min.y, max.z), dx, dy, mat));   // front
    }
    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override
    {
        return m_list.Hit(r, tMin, tMax, outRecord);
    }
    bool BoundingBox(AABB& outAABB) const override
    {
        outAABB = AABB(m_min, m_max);
        return true;
    }

private:
    glm::vec3 m_min, m_max;
    HittableList m_list;
    Material* m_material;
};


#endif
