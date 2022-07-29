#pragma once
#include "common.h"
#include "Ray.h"

namespace Library
{
	class Camera final
	{
	public:
		Camera(const pos3& pos, const precision& vertical_fov_degrees, const precision& aperture = 0, const precision& focus_dist = 1.0);
		~Camera() = default;

		Ray get_ray(const precision& u, const precision& v) const;

		void Move(bool forward, bool back, bool left, bool right);
		void Rotate(const precision& x, const precision& y);

		bool WasUpdated();

	private:
		pos3 _position;
		vec3 _screen_horizontal;
		vec3 _screen_vertical;
		pos3 _screen_bottomleft;

		precision _focusdist;
		precision _viewport_height;
		precision _viewport_width;
		precision _lensradius;

		precision _yaw{ degrees_to_radians(-90.0) };
		precision _pitch{ 0 };
		vec3 _forward, _right, _up;

		const precision _movespeed{ 10 };
		const precision _rotspeed{ degrees_to_radians(5.0) };
		const vec3 vertical_up{ 0,1,0 };

		bool _updated{ false };
	};
}