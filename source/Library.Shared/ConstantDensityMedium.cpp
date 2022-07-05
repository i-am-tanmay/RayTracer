#include "pch.h"
#include "ConstantDensityMedium.h"

namespace Library
{

	bool ConstantDensityMedium::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		HitInfo hit_close, hit_far;

		if (!_container->hit(ray, -infinity, infinity, hit_close)) return false;
		if (!_container->hit(ray, hit_close.ray_t + .0001, infinity, hit_far)) return false;

		if (hit_close.ray_t < t_min) hit_close.ray_t = t_min;
		if (hit_far.ray_t > t_max) hit_close.ray_t = t_max;
		if (hit_close.ray_t >= hit_far.ray_t) return false;
		if (hit_close.ray_t < 0) hit_close.ray_t = 0;

		const precision ray_length = ray.direction().length();
		const precision dist_inside = (hit_far.ray_t - hit_close.ray_t) * ray_length;
		const precision dist_hit = _negativeinverse_density * std::log(get_random01());
		if (dist_hit > dist_inside) return false;

		hitinfo.ray_t = hit_close.ray_t + dist_hit / ray_length;
		hitinfo.pos = ray.at(hitinfo.ray_t);

		// FAKE
		hitinfo.normal = vec3{ 1,0,0 };
		hitinfo.isFrontFace = true;
		hitinfo.material = _phase_material;

		return true;
	}

	bool ConstantDensityMedium::aabb(AABB& out_box) const
	{
		return _container->aabb(out_box);
	}

	Library::precision ConstantDensityMedium::aabb_area() const
	{
		return _container->aabb_area();
	}

}