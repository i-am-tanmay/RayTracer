#pragma once
#include "common.h"
#include "Ray.h"

namespace Library
{
	class Camera final
	{
	public:
		Camera(pos3 lookfrom, pos3 lookat, precision vertical_fov_degrees, vec3 vertical_up = vec3{ 0,1,0 }, precision aperture = 0, precision focus_dist = 1.0);
		~Camera() = default;

		Ray get_ray(precision u, precision v) const;

	private:
		pos3 _origin;
		vec3 _screen_horizontal;
		vec3 _screen_vertical;
		pos3 _screen_bottomleft;

		precision _lensradius;
		vec3 _u, _v;
	};
}