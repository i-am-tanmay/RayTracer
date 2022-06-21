#pragma once

#include "common.h"
#include "vec3.h"
#include "Ray.h"
#include "Sphere.h"
#include "Material.h"
#include <iostream>
#include <cmath>
#include <functional>

namespace Library
{
	inline void write_color(std::uint8_t* out, const vec3& pixel_color, const precision& samples_inverse)
	{
		// sqrt for gamma 2.0 correction
		out[0] = static_cast<std::uint8_t>(256 * clamp(std::sqrt(pixel_color._vec[0] * samples_inverse), 0, 0.999));
		out[1] = static_cast<std::uint8_t>(256 * clamp(std::sqrt(pixel_color._vec[1] * samples_inverse), 0, 0.999));
		out[2] = static_cast<std::uint8_t>(256 * clamp(std::sqrt(pixel_color._vec[2] * samples_inverse), 0, 0.999));
		//out[3] = static_cast<std::uint8_t>(255);
	}

	color ray_color(const Ray& ray, const IRenderObject& world, std::size_t bounce_limit)
	{
		if (bounce_limit == 0) return color{ 0,0,0 };

		HitInfo hitinfo;
		if (world.hit(ray, 0.001, infinity, hitinfo))
		{
			Ray ray_scattered;
			color attenuation;
			color emission = hitinfo.material->emitted(hitinfo.u, hitinfo.v, hitinfo.pos);

			if (hitinfo.material->scatter(ray, hitinfo, attenuation, ray_scattered))
				return emission + attenuation * ray_color(ray_scattered, world, bounce_limit - 1);
			else
				return emission;
		}

		return color(0, 0, 0);

		// GRADIENT
		//vec3 direction = unit_vector(ray.direction());
		//precision t = 0.5 * (direction.y() + 1);
		//return (1 - t) * color { 1, 1, 1 } + t * color{ 0.5, 0.7, 1.0 };	// lerp white to blue
	}
}