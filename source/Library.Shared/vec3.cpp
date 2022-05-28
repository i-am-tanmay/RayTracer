#include "pch.h"
#include "vec3.h"

namespace Library
{
	bool vec3::is_near_zero() const
	{
		const precision threshold = 1e-8;
		return (fabs(_vec[0]) + fabs(_vec[1]) + fabs(_vec[2])) < threshold;
	}

	vec3 operator+(const vec3& lhs, const vec3& rhs) { return vec3(lhs._vec[0] + rhs._vec[0], lhs._vec[1] + rhs._vec[1], lhs._vec[2] + rhs._vec[2]); }
	vec3 operator-(const vec3& lhs, const vec3& rhs) { return vec3(lhs._vec[0] - rhs._vec[0], lhs._vec[1] - rhs._vec[1], lhs._vec[2] - rhs._vec[2]); }

	vec3 operator*(const vec3& lhs, const vec3& rhs) { return vec3(lhs._vec[0] * rhs._vec[0], lhs._vec[1] * rhs._vec[1], lhs._vec[2] * rhs._vec[2]); }
	vec3 operator*(const precision& val, const vec3& vec) { return vec3(val * vec._vec[0], val * vec._vec[1], val * vec._vec[2]); }
	vec3 operator*(const vec3& vec, const precision& val) { return val * vec; }

	vec3 operator/(vec3 vec, const precision& val) { return (1 / val) * vec; }

	precision dot(const vec3& lhs, const vec3& rhs)
	{
		return lhs._vec[0] * rhs._vec[0]
			+ lhs._vec[1] * rhs._vec[1]
			+ lhs._vec[2] * rhs._vec[2];
	}

	vec3 cross(const vec3& lhs, const vec3& rhs)
	{
		return vec3(
			lhs._vec[1] * rhs._vec[2] - lhs._vec[2] * rhs._vec[1],
			lhs._vec[2] * rhs._vec[0] - lhs._vec[0] * rhs._vec[2],
			lhs._vec[0] * rhs._vec[1] - lhs._vec[1] * rhs._vec[0]
		);
	}

	vec3 unit_vector(const vec3& vec) { return vec / vec.length(); }

	vec3 random_in_unit_sphere()
	{
		while (true)
		{
			vec3 rend_vec = vec3::random(-1, 1);
			if (rend_vec.length_squared() < 1) return rend_vec;
		}
	}

	vec3 random_unit_vector()
	{
		return unit_vector(random_in_unit_sphere());
	}

	vec3 random_in_unit_hemisphere(const vec3& normal)
	{
		vec3 in_unit_sphere = random_in_unit_sphere();
		if (dot(in_unit_sphere, normal) > 0.0) return in_unit_sphere; // In the same hemisphere as the normal
		else return -in_unit_sphere;
	}

	vec3 reflect(const vec3& direction, const vec3& normal)
	{
		return direction - 2 * dot(direction, normal) * normal;
	}
}