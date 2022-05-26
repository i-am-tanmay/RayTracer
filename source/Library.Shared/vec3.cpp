#include "pch.h"
#include "vec3.h"

namespace Library
{
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
}