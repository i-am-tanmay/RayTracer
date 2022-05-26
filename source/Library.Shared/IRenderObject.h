#pragma once

#include "Ray.h"

namespace Library
{
	struct HitInfo
	{
		pos3 hitpos;
		precision ray_t;

		vec3 normal;
		bool _isFrontFace;

		inline void hit_updatenormal(const Ray& ray, const vec3& outward_normal)
		{
			_isFrontFace = dot(ray.direction(), outward_normal) < 0;
			normal = _isFrontFace ? outward_normal : -outward_normal;
		}
	};

	class IRenderObject
	{
	public:
		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& rec) const = 0;
		virtual ~IRenderObject() = default;
	};
}