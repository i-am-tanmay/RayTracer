#include "pch.h"
#include "Rect.h"

namespace Library
{
	bool Rect_XY::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		const vec3& ray_origin = ray.origin();
		const vec3& ray_direction = ray.direction();

		precision t = (_z - ray_origin.z()) / ray_direction.z();
		if (t < t_min || t > t_max) return false;

		precision x = ray_origin.x() + t * ray_direction.x();
		precision y = ray_origin.y() + t * ray_direction.y();
		if (x < _xmin || x > _xmax || y < _ymin || y > _ymax) return false;

		hitinfo.u = (x - _xmin) * _x_inverse;
		hitinfo.v = (y - _ymin) * _y_inverse;
		hitinfo.ray_t = t;
		hitinfo.hit_updatenormal(ray, vec3{ 0, 0, 1 });
		hitinfo.material = _mat;
		hitinfo.pos = ray.at(t);

		return true;
	}

	bool Rect_XY::aabb(AABB& out_box) const
	{
		out_box = AABB{ pos3{_xmin, _ymin, _z - 0.0001}, pos3{_xmax, _ymax, _z + 0.0001} };
		return true;
	}

	precision Rect_XY::aabb_area() const
	{
		precision l = _xmax - _xmin;
		precision b = _ymax - _ymin;
		precision h = 0.0002;
		return 2 * (l * b + b * h + h * l);
	}

	bool Rect_XZ::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		const vec3& ray_origin = ray.origin();
		const vec3& ray_direction = ray.direction();

		precision t = (_y - ray_origin.y()) / ray_direction.y();
		if (t < t_min || t > t_max) return false;

		precision x = ray_origin.x() + t * ray_direction.x();
		precision z = ray_origin.z() + t * ray_direction.z();
		if (x < _xmin || x > _xmax || z < _zmin || z > _zmax) return false;

		hitinfo.u = (x - _xmin) * _x_inverse;
		hitinfo.v = (z - _zmin) * _z_inverse;
		hitinfo.ray_t = t;
		hitinfo.hit_updatenormal(ray, vec3{ 0, 1, 0 });
		hitinfo.material = _mat;
		hitinfo.pos = ray.at(t);

		return true;
	}

	bool Rect_XZ::aabb(AABB& out_box) const
	{
		out_box = AABB{ pos3{_xmin, _y - 0.0001, _zmin }, pos3{_xmax, _y + 0.0001, _zmax} };
		return true;
	}

	precision Rect_XZ::aabb_area() const
	{
		precision l = _xmax - _xmin;
		precision b = _zmax - _zmin;
		precision h = 0.0002;
		return 2 * (l * b + b * h + h * l);
	}

	bool Rect_YZ::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		const vec3& ray_origin = ray.origin();
		const vec3& ray_direction = ray.direction();

		precision t = (_x - ray_origin.x()) / ray_direction.x();
		if (t < t_min || t > t_max) return false;

		precision y = ray_origin.y() + t * ray_direction.y();
		precision z = ray_origin.z() + t * ray_direction.z();
		if (y < _ymin || y > _ymax || z < _zmin || z > _zmax) return false;

		hitinfo.u = (y - _ymin) * _y_inverse;
		hitinfo.v = (z - _zmin) * _z_inverse;
		hitinfo.ray_t = t;
		hitinfo.hit_updatenormal(ray, vec3{ 1, 0, 0 });
		hitinfo.material = _mat;
		hitinfo.pos = ray.at(t);

		return true;
	}

	bool Rect_YZ::aabb(AABB& out_box) const
	{
		out_box = AABB{ pos3{_x - 0.0001, _ymin, _zmin}, pos3{_x + 0.0001, _ymax, _zmax} };
		return true;
	}

	precision Rect_YZ::aabb_area() const
	{
		precision l = _ymax - _ymin;
		precision b = _zmax - _zmin;
		precision h = 0.0002;
		return 2 * (l * b + b * h + h * l);
	}
}