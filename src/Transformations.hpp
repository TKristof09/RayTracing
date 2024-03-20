#pragma once

#include "Hittable.h"
#include "AABB.h"
class Translate : public Hittable
{
public:
    Translate(Hittable* obj, const glm::vec3& offset) : m_obj(obj), m_offset(offset) {}
    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override
    {
        Ray movedR(r.GetOrigin() - m_offset, r.GetDir());
        if(!m_obj->Hit(movedR, tMin, tMax, outRecord))
            return false;

        outRecord.point += m_offset;
        return true;
    }
    bool BoundingBox(AABB& outAABB) const override
    {
        if(!m_obj->BoundingBox(outAABB))
            return false;
        outAABB = AABB(outAABB.GetMin() + m_offset, outAABB.GetMax() + m_offset);
        return true;
    }

private:
    Hittable* m_obj;
    glm::vec3 m_offset;
};

class RotateY : public Hittable
{
public:
    RotateY(Hittable* obj, float angle)
    {
        m_obj         = obj;
        float radians = glm::radians(angle);
        m_sinTheta    = glm::sin(radians);
        m_cosTheta    = glm::cos(radians);

        m_obj->BoundingBox(m_bbox);
        glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
        glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for(int i = 0; i < 2; i++)
            for(int j = 0; j < 2; j++)
                for(int k = 0; k < 2; k++)
                {
                    float x    = i * m_bbox.GetMax().x + (1 - i) * m_bbox.GetMin().x;
                    float y    = j * m_bbox.GetMax().y + (1 - j) * m_bbox.GetMin().y;
                    float z    = k * m_bbox.GetMax().z + (1 - k) * m_bbox.GetMin().z;
                    float newX = m_cosTheta * x + m_sinTheta * z;
                    float newZ = -m_sinTheta * x + m_cosTheta * z;
                    glm::vec3 tester(newX, y, newZ);
                    for(int c = 0; c < 3; c++)
                    {
                        if(tester[c] > max[c])
                            max[c] = tester[c];
                        if(tester[c] < min[c])
                            min[c] = tester[c];
                    }
                }
        m_bbox = AABB(min, max);
    }
    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override
    {
        glm::vec3 origin    = r.GetOrigin();
        glm::vec3 direction = r.GetDir();

        // change from world space to object space
        origin[0] = m_cosTheta * r.GetOrigin()[0] - m_sinTheta * r.GetOrigin()[2];
        origin[2] = m_sinTheta * r.GetOrigin()[0] + m_cosTheta * r.GetOrigin()[2];

        direction[0] = m_cosTheta * r.GetDir()[0] - m_sinTheta * r.GetDir()[2];
        direction[2] = m_sinTheta * r.GetDir()[0] + m_cosTheta * r.GetDir()[2];

        Ray rotatedR(origin, direction);


        if(!m_obj->Hit(rotatedR, tMin, tMax, outRecord))
            return false;

        // change from object space back to world space
        glm::vec3 p      = outRecord.point;
        glm::vec3 normal = outRecord.normal;

        p[0] = m_cosTheta * outRecord.point[0] + m_sinTheta * outRecord.point[2];
        p[2] = -m_sinTheta * outRecord.point[0] + m_cosTheta * outRecord.point[2];

        normal[0] = m_cosTheta * outRecord.normal[0] + m_sinTheta * outRecord.normal[2];
        normal[2] = -m_sinTheta * outRecord.normal[0] + m_cosTheta * outRecord.normal[2];

        outRecord.point  = p;
        outRecord.normal = normal;
        return true;
    }
    bool BoundingBox(AABB& outAABB) const override
    {
        outAABB = m_bbox;
        return true;
    }

private:
    Hittable* m_obj;
    float m_sinTheta;
    float m_cosTheta;
    AABB m_bbox;
};
