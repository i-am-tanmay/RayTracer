#pragma once
#include "vec3.h"

// utility

inline vec3 operator+(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }
inline vec3 operator-(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }

inline vec3 operator*(const vec3& lhs, const vec3& rhs) { return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); }
inline vec3 operator*(const precision& val, const vec3& vec) { return vec3(val * vec.x, val * vec.y, val * vec.z); }
inline vec3 operator*(const vec3& vec, const precision& val) { return val * vec; }

inline vec3 operator/(vec3 vec, const precision& val) { return (1 / val) * vec; }

inline precision dot(const vec3& lhs, const vec3& rhs)
{
	return lhs.x * rhs.x
		+ lhs.y * rhs.y
		+ lhs.z * rhs.z;
}

inline vec3 cross(const vec3& lhs, const vec3& rhs)
{
	return vec3(
		lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x
	);
}

inline vec3 unit_vector(const vec3& vec) { return vec / vec.length(); }