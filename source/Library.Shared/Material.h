#pragma once
#include "common.h"
#include <functional>

namespace Library
{
	struct HitInfo;
	class Ray;
	class vec3;

	class Material
	{
	public:
		virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const = 0;
		virtual ~Material() = default;
	};
}