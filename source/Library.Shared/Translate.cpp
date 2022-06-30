#include "pch.h"
#include "Translate.h"

namespace Library
{
	bool Translate::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		Ray translated_ray{ ray.origin() - _offset, ray.direction() };
		if (!_object->hit(translated_ray, t_min, t_max, hitinfo)) return false;

		hitinfo.pos += _offset;
		hitinfo.hit_updatenormal(translated_ray, hitinfo.normal);
		return true;
	}

	bool Translate::aabb(AABB& out_box) const
	{
		if (!_object->aabb(out_box)) return false;

		out_box = AABB
		{
			out_box.min() + _offset,
			out_box.max() + _offset
		};
		return true;
	}

	Library::precision Translate::aabb_area() const
	{
		return _object->aabb_area();
	}
}