#pragma once

#include "vec3.h"
#include <iostream>

template<typename T> void write_color(std::uint8_t* out, const vec3<T>& pixel_color)
{
	out[0] = static_cast<std::uint8_t>(T{ 255 } * pixel_color.x());
	out[1] = static_cast<std::uint8_t>(T{ 255 } * pixel_color.y());
	out[2] = static_cast<std::uint8_t>(T{ 255 } * pixel_color.z());
}