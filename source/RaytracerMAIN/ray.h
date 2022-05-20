#pragma once

#include "vec3.h"

class ray final
{
public:
	ray() = default;
	ray(const vec3& origin, const vec3& direction) :_origin{ origin }, _direction{ direction } {}

	ray(const ray&) = default;
	ray& operator=(const ray&) = default;
	ray(ray&&) noexcept = default;
	ray& operator=(ray&&) noexcept = default;

	const vec3& origin() const { return _origin; }
	const vec3& direction() const { return _direction; }

	vec3 at(const precision& val) const { return _origin + val * _direction; }

private:
	vec3 _origin;
	vec3 _direction;
};