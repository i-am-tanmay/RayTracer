#pragma once

#include "vec3.h"
#include "ray.h"
#include <iostream>

void write_color(std::uint8_t* out, const vec3& pixel_color)
{
	out[0] = static_cast<std::uint8_t>(precision{ 255 } * pixel_color.x);
	out[1] = static_cast<std::uint8_t>(precision{ 255 } * pixel_color.y);
	out[2] = static_cast<std::uint8_t>(precision{ 255 } * pixel_color.z);
}

color ray_color(const ray& r)
{
	// blue to white gradient?
	vec3 direction = unit_vector(r.direction());
	precision t = precision{ 0.5 } *(direction.y + 1);
	return (1 - t) * color { 1, 1, 1 } + t * color{ 0.5, 0.7, 1.0 };
}