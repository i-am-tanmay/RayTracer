#pragma once

#include "vec3.h"
#include "Ray.h"
#include "Sphere.h"
#include <iostream>
#include <cmath>

void write_color(std::uint8_t* out, const vec3& pixel_color)
{
	out[0] = static_cast<std::uint8_t>(precision{ 255.0 } *pixel_color.x);
	out[1] = static_cast<std::uint8_t>(precision{ 255.0 } *pixel_color.y);
	out[2] = static_cast<std::uint8_t>(precision{ 255.0 } *pixel_color.z);
}

color ray_color(const Ray& ray)
{
	Sphere sphere{ pos3{ 0, 0, -1.0 }, 0.5 };
	HitInfo hitinfo;
	if (sphere.hit(ray, 0, 1, hitinfo)) return 0.5 * color{ hitinfo.normal.x + 1, hitinfo.normal.y + 1, hitinfo.normal.z + 1 };

	// GRADIENT
	vec3 direction = unit_vector(ray.direction);
	precision t = precision{ 0.5 } *(direction.y + 1);
	return (1 - t) * color { 1, 1, 1 } + t * color{ 0.5, 0.7, 1.0 };	// lerp white to blue
}