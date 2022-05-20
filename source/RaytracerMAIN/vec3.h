#pragma once

#include <cmath>
#include <iostream>

using precision = double;

class vec3 final
{
public:
	vec3() : _vec{ 0,0,0 } {}
	vec3(const precision& one, const precision& two, const precision& three) :_vec{ one, two, three } {}

	vec3(const vec3&) = default;
	vec3& operator=(const vec3&) = default;
	vec3(vec3&&) noexcept = default;
	vec3& operator=(vec3&&) noexcept = default;

	const precision& x{ _vec[0] };
	const precision& r{ _vec[0] };

	const precision& y{ _vec[1] };
	const precision& g{ _vec[1] };

	const precision& z{ _vec[2] };
	const precision& b{ _vec[2] };

	vec3 operator-() const { return vec3(-_vec[0], -_vec[1], -_vec[2]); }

	precision& operator[](int i) { return _vec[i]; }
	const precision& operator[](int i) const { return _vec[i]; }

	vec3& operator+=(const vec3& rhs)
	{
		_vec[0] += rhs._vec[0];
		_vec[1] += rhs._vec[1];
		_vec[2] += rhs._vec[2];

		return *this;
	}

	vec3& operator*=(const precision& val)
	{
		_vec[0] *= val;
		_vec[1] *= val;
		_vec[2] *= val;
		return *this;
	}

	vec3& operator/=(const precision& val)
	{
		return this->operator*=(1 / val);
	}

	precision length() const { return std::sqrt(_vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2]); }
	precision length_squared() const { return _vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2]; }

private:
	precision _vec[3];
};

#include "vec3.inl"

// alias
using color = vec3;
using pos3 = vec3;