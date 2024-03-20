#ifndef MATERIAL_H
#define MATERIAL_H

#include "glm/glm.hpp"
#include "Ray.h"
#include "Hittable.h"
#include "3DMath/Random.h"
#include "Texture.h"
#include "Allocator.hpp"

class Material
{
public:
    virtual bool Scatter(const Ray& r, const HitRecord& rec, glm::vec3& attenuation, Ray& rOut) const = 0;

    virtual glm::vec3 Emitted(const glm::vec2& uv, const glm::vec3& point)
    {
        return glm::vec3(0);
    }
};


class Lambertian : public Material
{
public:
    Lambertian(const glm::vec3& albedo) : m_texture(g_materialAllocator.Allocate<SolidColor>(albedo)) {}
    Lambertian(Texture* t) : m_texture(t) {}
    virtual bool Scatter(const Ray& r, const HitRecord& rec, glm::vec3& outAttenuation, Ray& outRay) const override
    {
        // glm::vec3 v = glm::RandomOnUnitSphere<float>();
        // auto scatterDir = (glm::dot(v, rec.normal) > 0.0 ? v : -v);
        auto scatterDir = rec.normal + math::RandomOnUnitSphere<float>();
        if(glm::dot(scatterDir, scatterDir) < 0.00001)
            scatterDir = rec.normal;
        // if(!(scatterDir[0] > 0 && scatterDir[1] > 0 && scatterDir[2] > 0))
        //	std::cout << "now" << std::endl;
        outRay         = Ray(rec.point + rec.normal * 0.0001f, scatterDir);
        outAttenuation = m_texture->Sample(rec.uv, rec.point);

        return true;
    }

private:
    Texture* m_texture;
};

class Metal : public Material
{
public:
    Metal(const glm::vec3& albedo, float fuzziness) : m_albedo(g_materialAllocator.Allocate<SolidColor>(albedo)), m_fuzziness(fuzziness < 1 ? fuzziness : 1) {}

    Metal(Texture* albedo, float fuzziness) : m_albedo(albedo), m_fuzziness(fuzziness < 1 ? fuzziness : 1) {}

    virtual bool Scatter(const Ray& r, const HitRecord& rec, glm::vec3& outAttenuation, Ray& outRay) const override
    {
        glm::vec3 reflected = glm::reflect(glm::normalize(r.GetDir()), rec.normal) + m_fuzziness * math::RandomInUnitSphere<float>();
        outRay              = Ray(rec.point, reflected);
        outAttenuation      = m_albedo->Sample(rec.uv, rec.point);

        return (glm::dot(reflected, rec.normal)) > 0;
    }

private:
    Texture* m_albedo;
    float m_fuzziness;
};

class Dielectric : public Material
{
public:
    Dielectric(float refractionIndex) : m_refractionIndex(refractionIndex) {}

    virtual bool Scatter(const Ray& r, const HitRecord& rec, glm::vec3& outAttenuation, Ray& outRay) const override
    {
        outAttenuation       = glm::vec3(1);
        float etaI_over_etaR = rec.frontFace ? (1.0 / m_refractionIndex) : m_refractionIndex;

        glm::vec3 unitDir = glm::normalize(r.GetDir());
        float cosTheta    = fmin(glm::dot(-unitDir, rec.normal), 1.0);
        float sinThetaSq  = 1.0 - cosTheta * cosTheta;

        bool cannotRefract = etaI_over_etaR * etaI_over_etaR * sinThetaSq > 1.0;

        glm::vec3 direction;

        if(cannotRefract || reflectance(cosTheta, etaI_over_etaR) > math::RandomReal<float>())
            direction = glm::reflect(unitDir, rec.normal);
        else
            direction = glm::refract(unitDir, rec.normal, etaI_over_etaR);

        outRay = Ray(rec.point, direction);
        return true;
    }

private:
    // Shlick's approximation
    static float reflectance(float cos, float etaI_over_etaR)
    {
        auto r0 = (1 - etaI_over_etaR) / (1 + etaI_over_etaR);
        r0      = r0 * r0;

        float x = 1 - cos;
        return r0 + (1 - r0) * x * x * x * x * x;
    }


    float m_refractionIndex;
};

class Emissive : public Material
{
public:
    Emissive(Texture* t) : m_emissionTexture(t) {}
    Emissive(const glm::vec3& color) : m_emissionTexture(g_materialAllocator.Allocate<SolidColor>(color)) {}
    virtual bool Scatter(const Ray& r, const HitRecord& rec, glm::vec3& outAttenuation, Ray& outRay) const override
    {
        return false;
    }

    virtual glm::vec3 Emitted(const glm::vec2& uv, const glm::vec3& point) override
    {
        return m_emissionTexture->Sample(uv, point);
    }

private:
    Texture* m_emissionTexture;
};
#endif
