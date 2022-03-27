#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "Hittable.h"
#include "3DMath/Random.h"
#include "Texture.h"

class Material
{
public:
	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& attenuation, Ray& rOut) const = 0;

	virtual math::Vec3d Emitted(const math::Vec2d& uv, const math::Vec3d& point)
	{
		return math::Vec3d(0);
	}
};



class Lambertian : public Material
{
public:
	Lambertian(const math::Vec3d& albedo):
		m_texture(std::make_shared<SolidColor>(albedo)) {}
	Lambertian(std::shared_ptr<Texture> t):
		m_texture(t) {}
	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& outAttenuation, Ray& outRay) const override
	{
		auto scatterDir = rec.normal + math::RandomOnUnitSphere<double>();
		if(math::NearZero(scatterDir))
			scatterDir = rec.normal;
		outRay = Ray(rec.point, scatterDir);
		outAttenuation = m_texture->Sample(rec.uv, rec.point);

		return true;
	}

private:
	std::shared_ptr<Texture> m_texture;
};

class Metal : public Material
{
public:
	Metal(const math::Vec3d& albedo, double fuzziness):
		m_albedo(std::make_shared<SolidColor>(albedo)), m_fuzziness(fuzziness < 1 ? fuzziness : 1) {}

	Metal(std::shared_ptr<Texture> albedo, double fuzziness):
		m_albedo(albedo), m_fuzziness(fuzziness < 1 ? fuzziness : 1) {}

	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& outAttenuation, Ray& outRay) const override
	{
		math::Vec3d reflected = math::reflect(math::normalize(r.GetDir()), rec.normal) + m_fuzziness * math::RandomInUnitSphere<double>();
		outRay = Ray(rec.point, reflected);
		outAttenuation = m_albedo->Sample(rec.uv, rec.point);

		return (math::dot(reflected, rec.normal)) > 0;
	}
private:
	std::shared_ptr<Texture> m_albedo;
	double m_fuzziness;
};

class Dielectric : public Material
{
public:
	Dielectric(double refractionIndex):
		m_refractionIndex(refractionIndex) {}

	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& outAttenuation, Ray& outRay) const override
	{
		outAttenuation = math::Vec3d(1);
		double etaI_over_etaR = rec.frontFace ? (1.0 / m_refractionIndex) : m_refractionIndex;

		math::Vec3d unitDir = math::normalize(r.GetDir());
		double cosTheta = fmin(dot(-unitDir, rec.normal), 1.0);
		double sinThetaSq = 1.0 - cosTheta * cosTheta;

		bool cannotRefract = etaI_over_etaR * etaI_over_etaR * sinThetaSq > 1.0;

		math::Vec3d direction;

		if(cannotRefract || reflectance(cosTheta, etaI_over_etaR) > math::RandomReal<double>())
			direction = math::reflect(unitDir, rec.normal);
		else
			direction = math::refract(unitDir, rec.normal, etaI_over_etaR);

		outRay = Ray(rec.point, direction);
		return true;
	}
private:
	// Shlick's approximation
	static double reflectance(double cos, double etaI_over_etaR)
	{
		auto r0 = (1 - etaI_over_etaR) / (1 + etaI_over_etaR);
		r0 = r0 * r0;

		double x = 1 - cos;
		return r0 + (1 - r0) * x * x * x * x * x;
	}


	double m_refractionIndex;
};

class Emissive : public Material
{
public:
	Emissive(std::shared_ptr<Texture> t):
		m_emissionTexture(t) {}
	Emissive(const math::Vec3d& color):
		m_emissionTexture(std::make_shared<SolidColor>(color)) {}
	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& outAttenuation, Ray& outRay) const override
	{
		return false;
	}

	virtual math::Vec3d Emitted(const math::Vec2d& uv, const math::Vec3d& point)
	{
		return m_emissionTexture->Sample(uv, point);
	}

private:
	std::shared_ptr<Texture> m_emissionTexture;
};
#endif
