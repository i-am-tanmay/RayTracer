#pragma once

#include "vec3.h"

class Ray final
{
public:
	Ray() = default;
	Ray(const vec3& origin, const vec3& direction) :_origin{ origin }, _direction{ direction } {}

	Ray(const Ray&) = default;
	Ray& operator=(const Ray&) = default;
	Ray(Ray&&) noexcept = default;
	Ray& operator=(Ray&&) noexcept = default;

	const vec3& origin{ _origin };
	const vec3& direction{ _direction };

	vec3 at(const precision& val) const { return _origin + val * _direction; }

private:
	vec3 _origin;
	vec3 _direction;
};