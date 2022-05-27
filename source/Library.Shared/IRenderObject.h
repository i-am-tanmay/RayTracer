#pragma once

#include "Ray.h"

namespace Library
{
	class Material;

	struct HitInfo
	{
		pos3 pos;
		precision ray_t;

		vec3 normal;
		bool isFrontFace;

		std::shared_ptr<Material> material;

		inline void hit_updatenormal(const Ray& ray, const vec3& outward_normal)
		{
			isFrontFace = dot(ray.direction(), outward_normal) < 0;
			normal = isFrontFace ? outward_normal : -outward_normal;
		}
	};

	class IRenderObject
	{
	public:
		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& rec) const = 0;
		virtual ~IRenderObject() = default;
	};
}