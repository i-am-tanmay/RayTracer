#pragma once
#include "common.h"
#include "Ray.h"

namespace Library
{
	class Camera final
	{
	public:
		Camera(const pos3& lookfrom, const pos3& lookat, const precision& vertical_fov_degrees, const pos3& vertical_up = vec3{ 0,1,0 }, const precision& aperture = 0, const precision& focus_dist = 1.0);
		~Camera() = default;

		Ray get_ray(const precision& u, const precision& v) const;

	private:
		pos3 _origin;
		vec3 _screen_horizontal;
		vec3 _screen_vertical;
		pos3 _screen_bottomleft;

		precision _lensradius;
		vec3 _u, _v;
	};
}