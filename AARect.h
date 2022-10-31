#ifndef AARECT_H
#define AARECT_H

#include "Hittable.h"
#include "AABB.h"

class XY_Rect : public Hittable
{
public:
	XY_Rect() {}
	XY_Rect(const math::Vec2d& min, const math::Vec2d& max, double z, std::shared_ptr<Material> mat):
		m_min(min), m_max(max), m_z(z), m_material(mat) {}

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const override
	{
		auto t = (m_z - r.GetOrigin().z) * r.GetInvDir().z;
		if(t < tMin || t > tMax)
			return false;

		math::Vec2d xy = r.GetOrigin() + t * r.GetDir();
		if(xy.x < m_min.x || xy.x > m_max.x || xy.y < m_min.y || xy.y > m_max.y)
			return false;
		outRecord.uv.x = (xy.x - m_min.x) / (m_max.x - m_min.x);
		outRecord.uv.y = (xy.y - m_min.y) / (m_max.y - m_min.y);
		outRecord.t = t;
		outRecord.SetNormal(r, math::Vec3d(0, 0, 1));
		outRecord.material = m_material;
		outRecord.point = r.At(t);
		return true;
	}
	virtual bool BoundingBox(AABB& outAABB) const override
	{
		outAABB = AABB(math::Vec3d(m_min, m_z - 0.00001), math::Vec3d(m_max, m_z + 0.00001));
		return true;
	}
private:
	math::Vec2d m_min, m_max;
	double m_z;
	std::shared_ptr<Material> m_material;
};
class XZ_Rect : public Hittable
{
public:
	XZ_Rect() {}
	XZ_Rect(const math::Vec2d& min, const math::Vec2d& max, double y, std::shared_ptr<Material> mat):
		m_min(min), m_max(max), m_y(y), m_material(mat) {}

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const override
	{
		auto t = (m_y - r.GetOrigin().y) * r.GetInvDir().y;
		if(t < tMin || t > tMax)
			return false;

		double x = (r.GetOrigin().x + t * r.GetDir().x);
		double z = (r.GetOrigin().z + t * r.GetDir().z);
		if(x < m_min.x || x > m_max.x || z < m_min[1] || z > m_max[1])
			return false;
		outRecord.uv.x = (x - m_min.x) / (m_max.x - m_min.x);
		outRecord.uv.y = (z - m_min[1]) / (m_max[1] - m_min[1]);
		outRecord.t = t;
		outRecord.SetNormal(r, math::Vec3d(0, 1, 0));
		outRecord.material = m_material;
		outRecord.point = r.At(t);
		return true;
	}
	virtual bool BoundingBox(AABB& outAABB) const override
	{
		outAABB = AABB(math::Vec3d(m_min[0], m_y - 0.00001, m_min[1]), math::Vec3d(m_max[0], m_y + 0.00001, m_max[1]));
		return true;
	}
private:
	math::Vec2d m_min, m_max;
	double m_y;
	std::shared_ptr<Material> m_material;
};
class YZ_Rect : public Hittable
{
public:
	YZ_Rect() {}
	YZ_Rect(const math::Vec2d& min, const math::Vec2d& max, double x, std::shared_ptr<Material> mat):
		m_min(min), m_max(max), m_x(x), m_material(mat) {}

	virtual bool Hit(const Ray& r, double tMin, double tMax, HitRecord& outRecord) const override
	{
		auto t = (m_x - r.GetOrigin().x) * r.GetInvDir().x;
		if(t < tMin || t > tMax)
			return false;

		double y = (r.GetOrigin().y + t * r.GetDir().y);
		double z = (r.GetOrigin().z + t * r.GetDir().z);
		if(y < m_min[0] || y > m_max[0] || z < m_min[1] || z > m_max[1])
			return false;
		outRecord.uv.x = (y - m_min[0]) / (m_max[0] - m_min[0]);
		outRecord.uv.y = (z - m_min[1]) / (m_max[1] - m_min[1]);
		outRecord.t = t;
		outRecord.SetNormal(r, math::Vec3d(1, 0, 0));
		outRecord.material = m_material;
		outRecord.point = r.At(t);
		return true;
	}
	virtual bool BoundingBox(AABB& outAABB) const override
	{
		outAABB = AABB(math::Vec3d(m_x - 0.00001, m_min[0], m_min[1]), math::Vec3d(m_x + 0.00001, m_max[0], m_max[1]));
		return true;
	}
private:
	math::Vec2d m_min, m_max;
	double m_x;
	std::shared_ptr<Material> m_material;
};
#endif

