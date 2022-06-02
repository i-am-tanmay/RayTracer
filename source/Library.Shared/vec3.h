#pragma once
#include "common.h"

#include <cmath>
#include <iostream>

namespace Library
{
	class vec3 final
	{
	public:
		vec3() : _vec{ 0,0,0 } {}
		vec3(const precision& one, const precision& two, const precision& three) :_vec{ one, two, three } {}

		vec3(const vec3& rhs) = default;
		vec3& operator=(const vec3& rhs) = default;
		vec3(vec3&& rhs) noexcept = default;
		vec3& operator=(vec3&& rhs) noexcept = default;

		inline const precision& x() const { return _vec[0]; };
		inline const precision& r() const { return _vec[0]; };

		inline const precision& y() const { return _vec[1]; };
		inline const precision& g() const { return _vec[1]; };

		inline const precision& z() const { return _vec[2]; };
		inline const precision& b() const { return _vec[2]; };

		inline vec3 operator-() const { return vec3{ -_vec[0], -_vec[1], -_vec[2] }; }

		inline precision& operator[](int i) { return _vec[i]; }
		inline const precision& operator[](int i) const { return _vec[i]; }

		inline vec3& operator+=(const vec3& rhs)
		{
			_vec[0] += rhs._vec[0];
			_vec[1] += rhs._vec[1];
			_vec[2] += rhs._vec[2];

			return *this;
		}

		inline vec3& operator*=(const precision& val)
		{
			_vec[0] *= val;
			_vec[1] *= val;
			_vec[2] *= val;
			return *this;
		}

		inline vec3& operator/=(const precision& val)
		{
			return this->operator*=(1 / val);
		}

		inline precision length() const { return std::sqrt(_vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2]); }
		inline precision length_squared() const { return _vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2]; }

		inline static vec3 random() { return vec3(get_random01(), get_random01(), get_random01()); }
		inline static vec3 random(precision min_included, precision max_excluded) { return vec3(get_random(min_included, max_excluded), get_random(min_included, max_excluded), get_random(min_included, max_excluded)); }

		bool is_near_zero() const;

		friend void write_color(std::uint8_t* out, const vec3& pixel_color, std::size_t samples);
		friend vec3 operator+(const vec3& lhs, const vec3& rhs);
		friend vec3 operator-(const vec3& lhs, const vec3& rhs);
		friend vec3 operator*(const vec3& lhs, const vec3& rhs);
		friend vec3 operator*(const precision& val, const vec3& vec);
		friend vec3 operator*(const vec3& vec, const precision& val);
		friend precision dot(const vec3& lhs, const vec3& rhs);
		friend vec3 cross(const vec3& lhs, const vec3& rhs);
	private:
		precision _vec[3];
	};

	// utility

	vec3 operator+(const vec3& lhs, const vec3& rhs);
	vec3 operator-(const vec3& lhs, const vec3& rhs);

	vec3 operator*(const vec3& lhs, const vec3& rhs);
	vec3 operator*(const precision& val, const vec3& vec);
	vec3 operator*(const vec3& vec, const precision& val);

	vec3 operator/(vec3 vec, const precision& val);

	precision dot(const vec3& lhs, const vec3& rhs);
	vec3 cross(const vec3& lhs, const vec3& rhs);

	vec3 unit_vector(const vec3& vec);
	
	vec3 random_in_unit_sphere();
	vec3 random_unit_vector();
	vec3 random_in_unit_hemisphere(const vec3& normal);
	vec3 random_in_unit_circle();

	vec3 reflect(const vec3& v, const vec3& n);
	vec3 refract(const vec3& v, const vec3& n, precision index_of_refraction);

	// alias
	using color = vec3;
	using pos3 = vec3;
}