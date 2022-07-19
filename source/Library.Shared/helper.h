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
	inline void write_color(std::uint8_t* out, float* out_float, const vec3& pixel_color, const precision& samples_inverse)
	{
		// sqrt for gamma 2.0 correction
		out_float[0] = static_cast<float>(clamp(std::sqrt(pixel_color._vec[0] * samples_inverse), 0, 0.999));
		out_float[1] = static_cast<float>(clamp(std::sqrt(pixel_color._vec[1] * samples_inverse), 0, 0.999));
		out_float[2] = static_cast<float>(clamp(std::sqrt(pixel_color._vec[2] * samples_inverse), 0, 0.999));

		out[0] = static_cast<std::uint8_t>(256 * out_float[0]);
		out[1] = static_cast<std::uint8_t>(256 * out_float[1]);
		out[2] = static_cast<std::uint8_t>(256 * out_float[2]);
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

			return emission;
		}

		return vec3{ 0,0,0 };

		// GRADIENT
		//vec3 direction = unit_vector(ray.direction());
		//precision t = 0.5 * (direction.y() + 1);
		//return (1 - t) * color { 1, 1, 1 } + t * color{ 0.5, 0.7, 1.0 };	// lerp white to blue
	}

	void ray_color(const Ray& ray, const IRenderObject& world, float* denoise_normal, float* denoise_albedo)
	{
		HitInfo hitinfo;
		if (world.hit(ray, 0.001, infinity, hitinfo))
		{
			Ray ray_scattered;
			color attenuation;
			color emission = hitinfo.material->emitted(hitinfo.u, hitinfo.v, hitinfo.pos);

			vec3 normal = unit_vector(hitinfo.normal);
			denoise_normal[0] = static_cast<float>(normal.x());
			denoise_normal[1] = static_cast<float>(normal.y());
			denoise_normal[2] = static_cast<float>(normal.z());

			vec3 albedo = hitinfo.material->denoise_albedo(hitinfo.u, hitinfo.v, hitinfo.pos);
			denoise_albedo[0] = static_cast<float>(albedo.x());
			denoise_albedo[1] = static_cast<float>(albedo.y());
			denoise_albedo[2] = static_cast<float>(albedo.z());

		}

		denoise_albedo[0] = 0.f;
		denoise_albedo[1] = 0.f;
		denoise_albedo[2] = 0.f;

		denoise_normal[0] = 0.f;
		denoise_normal[1] = 0.f;
		denoise_normal[2] = 1.f;
	}
}