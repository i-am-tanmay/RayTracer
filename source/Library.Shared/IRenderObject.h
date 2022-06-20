#pragma once

#include "Ray.h"
#include "AABB.h"

namespace Library
{
	class Material;

	struct HitInfo
	{
		pos3 pos;
		precision ray_t{ 0 };

		precision u{ 0 };
		precision v{ 0 };

		vec3 normal;
		bool isFrontFace{ false };

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
		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const = 0;
		virtual bool aabb(AABB& out_box) const = 0;
		virtual precision aabb_area() const = 0;

		virtual ~IRenderObject() = default;
	};
}