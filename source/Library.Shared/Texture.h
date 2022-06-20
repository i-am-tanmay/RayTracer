#pragma once

#include "vec3.h"

namespace Library
{
	class Texture
	{
	public:
		virtual color get_color(const precision& u, const precision& v, const pos3& pos) const = 0;
	};

	class SolidColor final : public Texture
	{
	public:
		SolidColor() = default;
		SolidColor(const color& c) : _color_value(c) {}
		SolidColor(const precision& red, const precision& green, const precision& blue) : _color_value{ red, green, blue } {}

		virtual color get_color(const precision&, const precision&, const vec3&) const override { return _color_value; }

	private:
		color _color_value;
	};
}