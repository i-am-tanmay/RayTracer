#pragma once

#include "common.h"
#include "vec3.h"
#include "Ray.h"
#include "Sphere.h"
#include <iostream>
#include <cmath>

namespace Library
{
	void write_color(std::uint8_t* out, const vec3& pixel_color, std::size_t samples)
	{
		const precision scale{ 1.0 / samples };

		// sqrt for gamma 2.0 correction
		out[0] = static_cast<std::uint8_t>(256 * clamp(std::sqrt(pixel_color._vec[0] * scale), 0, 0.999));
		out[1] = static_cast<std::uint8_t>(256 * clamp(std::sqrt(pixel_color._vec[1] * scale), 0, 0.999));
		out[2] = static_cast<std::uint8_t>(256 * clamp(std::sqrt(pixel_color._vec[2] * scale), 0, 0.999));
	}

	color ray_color_cos3(const Ray& ray, const IRenderObject& world, std::size_t bounce_limit)
	{
		if (bounce_limit == 0) return color{ 0,0,0 };

		HitInfo hitinfo;
		if (world.hit(ray, 0.001, infinity, hitinfo))
		{
			pos3 target = hitinfo.pos + hitinfo.normal + random_in_unit_sphere();
			return 0.5 * ray_color_cos3(Ray{ hitinfo.pos, target - hitinfo.pos }, world, bounce_limit - 1);
		}

		// GRADIENT
		vec3 direction = unit_vector(ray.direction());
		precision t = 0.5 * (direction.y() + 1);
		return (1 - t) * color { 1, 1, 1 } + t * color{ 0.5, 0.7, 1.0 };	// lerp white to blue
	}

	color ray_color_cos(const Ray& ray, const IRenderObject& world, std::size_t bounce_limit)
	{
		if (bounce_limit == 0) return color{ 0,0,0 };

		HitInfo hitinfo;
		if (world.hit(ray, 0.001, infinity, hitinfo))
		{
			pos3 target = hitinfo.pos + hitinfo.normal + random_unit_vector();
			return 0.5 * ray_color_cos(Ray{ hitinfo.pos, target - hitinfo.pos }, world, bounce_limit - 1);
		}

		// GRADIENT
		vec3 direction = unit_vector(ray.direction());
		precision t = 0.5 * (direction.y() + 1);
		return (1 - t) * color { 1, 1, 1 } + t * color{ 0.5, 0.7, 1.0 };	// lerp white to blue
	}

	color ray_color_hemispherical(const Ray& ray, const IRenderObject& world, std::size_t bounce_limit)
	{
		if (bounce_limit == 0) return color{ 0,0,0 };

		HitInfo hitinfo;
		if (world.hit(ray, 0.001, infinity, hitinfo))
		{
			pos3 target = hitinfo.pos + random_in_unit_hemisphere(hitinfo.normal);
			return 0.5 * ray_color_hemispherical(Ray{ hitinfo.pos, target - hitinfo.pos }, world, bounce_limit - 1);
		}

		// GRADIENT
		vec3 direction = unit_vector(ray.direction());
		precision t = 0.5 * (direction.y() + 1);
		return (1 - t) * color { 1, 1, 1 } + t * color{ 0.5, 0.7, 1.0 };	// lerp white to blue
	}
}