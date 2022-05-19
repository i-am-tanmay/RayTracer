#pragma once

#include <cmath>
#include <iostream>

template<typename T>
class vec3 final
{
public:
	vec3() : _vec{ 0,0,0 } {}
	vec3(const T& one, const T& two, const T& three) :_vec{ one, two, three } {}

	vec3(const vec3&) = default;
	vec3& operator=(const vec3&) = default;
	vec3(vec3&&) noexcept = default;
	vec3& operator=(vec3&&) noexcept = default;

	T x() { return _vec[0]; }
	T r() { return _vec[0]; }
	const T& x() const { return _vec[0]; }
	const T& r() const { return _vec[0]; }

	T y() { return _vec[1]; }
	T g() { return _vec[1]; }
	const T& y() const { return _vec[1]; }
	const T& g() const { return _vec[1]; }

	T z() { return _vec[2]; }
	T b() { return _vec[2]; }
	const T& z() const { return _vec[2]; }
	const T& b() const { return _vec[2]; }

	vec3 operator-() const { return vec3(-_vec[0], -_vec[1], -_vec[2]); }

	T operator[](int i) const { return _vec[i]; }
	T& operator[](int i) { return _vec[i]; }

	vec3& operator+=(const vec3& v)
	{
		_vec[0] += v._vec[0];
		_vec[1] += v._vec[1];
		_vec[2] += v._vec[2];

		return *this;
	}

	vec3& operator*=(const T& val)
	{
		_vec[0] *= val;
		_vec[1] *= val;
		_vec[2] *= val;
		return *this;
	}

	vec3& operator/=(const T& val)
	{
		return this->operator*=(1 / val);
	}

	T length() const { return std::sqrt(_vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2]); }
	T length_squared() const { return _vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2]; }

private:
	T _vec[3];
};

// utility

template<typename T> inline std::ostream& operator<<(std::ostream& out, const vec3<T>& v) { return out << v._vec[0] << ' ' << v._vec[1] << ' ' << v._vec[2]; }

template<typename T> inline vec3<T> operator+(const vec3<T>& u, const vec3<T>& v) { return vec3<T>(u._vec[0] + v._vec[0], u._vec[1] + v._vec[1], u._vec[2] + v._vec[2]); }
template<typename T> inline vec3<T> operator-(const vec3<T>& u, const vec3<T>& v) { return vec3<T>(u._vec[0] - v._vec[0], u._vec[1] - v._vec[1], u._vec[2] - v._vec[2]); }

template<typename T> inline vec3<T> operator*(const vec3<T>& u, const vec3<T>& v) { return vec3<T>(u._vec[0] * v._vec[0], u._vec[1] * v._vec[1], u._vec[2] * v._vec[2]); }
template<typename T> inline vec3<T> operator*(const T& t, const vec3<T>& v) { return vec3<T>(t * v._vec[0], t * v._vec[1], t * v._vec[2]); }
template<typename T> inline vec3<T> operator*(const vec3<T>& v, T t) { return t * v; }

template<typename T> inline vec3<T> operator/(vec3<T> v, const T& t) { return (1 / t) * v; }

template<typename T> inline T dot(const vec3<T>& u, const vec3<T>& v)
{
	return u._vec[0] * v._vec[0]
		+ u._vec[1] * v._vec[1]
		+ u._vec[2] * v._vec[2];
}

template<typename T> inline vec3<T> cross(const vec3<T>& u, const vec3<T>& v)
{
	return vec3(
		u._vec[1] * v._vec[2] - u._vec[2] * v._vec[1],
		u._vec[2] * v._vec[0] - u._vec[0] * v._vec[2],
		u._vec[0] * v._vec[1] - u._vec[1] * v._vec[0]
	);
}

template<typename T> inline vec3<T> unit_vector(const vec3<T>& v) { return v / v.length(); }