#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "Hittable.h"
#include <vector>
#include "AABB.h"
#include "3DMath/Random.h"

class HittableList : public Hittable
{
public:
    HittableList() {}

    void Add(Hittable* obj)
    {
        m_objects.push_back(obj);
    }

    virtual bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override;
    virtual bool BoundingBox(AABB& outAABB) const override;

    virtual float PDFValue(const glm::vec3& origin, const glm::vec3& direction) const override
    {
        float weight = 1.0f / m_objects.size();
        float sum    = 0.f;
        for(const auto& obj : m_objects)
            sum += weight * obj->PDFValue(origin, direction);
        return sum;
    }

    virtual glm::vec3 Random(const glm::vec3& origin) const override
    {
        return m_objects[math::RandomInt(0, m_objects.size())]->Random(origin);
    }

    auto& GetObjects() { return m_objects; }

private:
    std::vector<Hittable*> m_objects;
};

inline bool HittableList::Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const
{
    HitRecord temp;
    bool hit      = false;
    float closest = tMax;

    for(const auto& obj : m_objects)
    {
        if(obj->Hit(r, tMin, closest, temp))
        {
            hit       = true;
            closest   = temp.t;
            outRecord = temp;
        }
    }

    return hit;
}
inline bool HittableList::BoundingBox(AABB& outAABB) const
{
    if(m_objects.empty())
        return false;

    AABB temp;
    bool firstIter = true;
    for(const auto& obj : m_objects)
    {
        if(!obj->BoundingBox(temp))
            return false;
        outAABB   = firstIter ? temp : SurroundingBox(temp, outAABB);
        firstIter = false;
    }
    return true;
}
#endif
