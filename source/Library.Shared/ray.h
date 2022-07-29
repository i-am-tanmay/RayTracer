#pragma once

#include "vec3.h"

namespace Library
{
	class Ray final
	{
	public:
		Ray() = default;
		Ray(const vec3& origin, const vec3& direction) :_position{ origin }, _direction{ direction } {}

		Ray(const Ray&) = default;
		Ray& operator=(const Ray&) = default;
		Ray(Ray&&) noexcept = default;
		Ray& operator=(Ray&&) noexcept = default;

		vec3& origin() { return _position; };
		const vec3& origin() const { return _position; };
		vec3& direction() { return _direction; };
		const vec3& direction() const { return _direction; };

		vec3 at(const precision& val) const { return _position + val * _direction; }

	private:
		vec3 _position;
		vec3 _direction;
	};
}