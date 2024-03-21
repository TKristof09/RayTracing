#pragma once
#include <iostream>
#include "Hittable.h"
#include "ONB.hpp"
#include "glm/ext/scalar_constants.hpp"
#include <glm/glm.hpp>
#include "3DMath/Random.h"

class PDF
{
public:
    virtual ~PDF()                                        = default;
    virtual float Value(const glm::vec3& direction) const = 0;
    virtual glm::vec3 Generate() const                    = 0;
};

class SpherePDF : public PDF
{
public:
    float Value(const glm::vec3& direction) const override
    {
        return 1.f / (4.f * glm::pi<float>());
    }
    glm::vec3 Generate() const override
    {
        return math::RandomOnUnitSphere<float>();
    }
};

class CosinePDF : public PDF
{
public:
    CosinePDF(const glm::vec3& normal) : m_onb(normal) {}
    float Value(const glm::vec3& direction) const override
    {
        float cosTheta = glm::dot(glm::normalize(direction), m_onb[2]);
        return cosTheta <= 0 ? 0 : cosTheta / glm::pi<float>();
    }

    glm::vec3 Generate() const override
    {
        return m_onb.Local(math::RandomCosineHemisphere<float>());
    }

private:
    ONB m_onb;
};

class HittablePDF : public PDF
{
public:
    HittablePDF(const glm::vec3& origin, const Hittable& object) : m_origin(origin), m_object(object) {}
    float Value(const glm::vec3& direction) const override
    {
        return m_object.PDFValue(m_origin, direction);
    }
    glm::vec3 Generate() const override
    {
        return m_object.Random(m_origin);
    }

private:
    glm::vec3 m_origin;
    const Hittable& m_object;
};

#define MIXTURE 0.2
class MixturePDF : public PDF
{
public:
    MixturePDF(PDF* p0, PDF* p1) : m_p0(p0), m_p1(p1) {}
    float Value(const glm::vec3& direction) const override
    {
        float v0 = m_p0->Value(direction);
        float v1 = m_p1->Value(direction);
        // std::cout << "v0: " << v0 << " v1: " << v1 << std::endl;
        return MIXTURE * v0 + (1 - MIXTURE) * v1;
    }
    glm::vec3 Generate() const override
    {
        float r = math::RandomReal<float>();
        // std::cout << "r: " << r << std::endl;
        return r < MIXTURE ? m_p0->Generate() : m_p1->Generate();
    }

private:
    PDF* m_p0;
    PDF* m_p1;
};
