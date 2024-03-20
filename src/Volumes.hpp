#pragma once

#include "Hittable.h"
#include "AABB.h"
#include "Texture.h"
#include "Material.h"
#include "Allocator.hpp"
#include "3DMath/Random.h"
#include <limits>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

class ConstantMedium : public Hittable
{
public:
    ConstantMedium(Hittable* boundary, float density, Texture* albedo) : m_boundary(boundary), m_negInvDensity(-1 / density)
    {
        m_phaseFunction = g_materialAllocator.Allocate<Isotropic>(albedo);
    }
    ConstantMedium(Hittable* boundary, float density, const glm::vec3& color) : m_boundary(boundary), m_negInvDensity(-1 / density)
    {
        m_phaseFunction = g_materialAllocator.Allocate<Isotropic>(color);
    }
    bool Hit(const Ray& r, float tMin, float tMax, HitRecord& outRecord) const override
    {
        HitRecord inHit, outHit;
        if(!m_boundary->Hit(r, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), inHit))
            return false;
        if(!m_boundary->Hit(r, inHit.t + 0.001f, std::numeric_limits<float>::max(), outHit))
            return false;

        if(inHit.t < tMin)
            inHit.t = tMin;
        if(outHit.t > tMax)
            outHit.t = tMax;
        if(inHit.t >= outHit.t)
            return false;
        if(inHit.t < 0)
            inHit.t = 0;

        float length                 = r.GetDir().length();
        float distanceInsideBoundary = (outHit.t - inHit.t) * length;
        float hitDistance            = m_negInvDensity * glm::log(math::RandomReal<float>());

        if(hitDistance > distanceInsideBoundary)
            return false;

        outRecord.t         = inHit.t + hitDistance / length;
        outRecord.point     = r.At(outRecord.t);
        outRecord.normal    = glm::vec3(1, 0, 0);  // arbitrary
        outRecord.frontFace = true;                // arbitrary
        outRecord.material  = m_phaseFunction;

        return true;
    }
    bool BoundingBox(AABB& outAABB) const override
    {
        return m_boundary->BoundingBox(outAABB);
    }

private:
    Hittable* m_boundary;
    float m_negInvDensity;
    Material* m_phaseFunction;
};
