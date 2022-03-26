#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "Hittable.h"
#include "3DMath/Random.h"

class Material
{
public:
	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& attenuation, Ray& rOut) const = 0;
};


class Lambertian : public Material
{
public:
	Lambertian(const math::Vec3d& albedo):
		m_albedo(albedo) {}
	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& outAttenuation, Ray& outRay) const override
	{
		auto scatterDir = rec.normal + math::RandomOnUnitSphere<double>();
		if(math::NearZero(scatterDir))
			scatterDir = rec.normal;
		outRay = Ray(rec.point, scatterDir);
		outAttenuation = m_albedo;

		return true;
	}

private:
	math::Vec3d m_albedo;
};

class Metal : public Material
{
public:
	Metal(const math::Vec3d& albedo, double fuzziness):
		m_albedo(albedo), m_fuzziness(fuzziness < 1 ? fuzziness : 1) {}

	virtual bool Scatter(const Ray& r, const HitRecord& rec, math::Vec3d& outAttenuation, Ray& outRay) const override
	{
		math::Vec3d reflected = math::reflect(math::normalize(r.GetDir()), rec.normal) + m_fuzziness * math::RandomInUnitSphere<double>();
		outRay = Ray(rec.point, reflected);
		outAttenuation = m_albedo;

		return (math::dot(reflected, rec.normal)) > 0;
	}
private:
	math::Vec3d m_albedo;
	double m_fuzziness;
};

#endif
