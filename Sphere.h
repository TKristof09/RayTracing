#ifndef SPHERE_H
#define SPHERE_H

#include "3DMath/3DMath.h"
#include "AABB.h"
#include "Hittable.h"

class Sphere : public Hittable
{
public:
	Sphere() {}
	Sphere(const math::Vec3d& center, double radius, std::shared_ptr<Material> material):
		m_center(center), m_radius(radius), m_material(material){}

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const override;
	virtual bool BoundingBox(AABB& outAABB) const override;
private:
	math::Vec3d m_center;
	double m_radius;
	std::shared_ptr<Material> m_material;
};

bool Sphere::Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const
{
	math::Vec3d oc = r.GetOrigin() - m_center;
	double a = math::lengthSq(r.GetDir());
	double half_b = math::dot(r.GetDir(), oc); // we can simplify the 2 in the return value calculation
	double c = math::lengthSq(oc) - m_radius * m_radius;

	double delta = half_b * half_b - a * c; // actually delta / 4 but doesn't matter

	if(delta < 0.0)
		return false;
	double sqDelta = sqrt(delta);
	double root = (-half_b - sqDelta) / a;

	if(root < tMin || root > tMax)
	{
		root = (-half_b + sqDelta) / a;
		if(root < tMin || root > tMax)
			return false;
	}

	outRecord.t = root;
	outRecord.point = r.At(root);
	outRecord.SetNormal(r, (outRecord.point - m_center) / m_radius);
	outRecord.material = m_material;
	return true;

}
bool Sphere::BoundingBox(AABB& outAABB) const
{
	outAABB = AABB(m_center - math::Vec3d(m_radius), m_center + math::Vec3d(m_radius));
	return true;
}


#endif
