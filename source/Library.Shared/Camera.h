#pragma once
#include "common.h"
#include "Ray.h"

namespace Library
{
	class Camera final
	{
	public:
		Camera(precision screen_height, precision focal_length, pos3 origin = pos3{ 0,0,0 });
		~Camera() = default;

		Ray get_ray(precision u, precision v) const;

	private:
		const pos3 _origin;
		const vec3 _screen_horizontal;
		const vec3 _screen_vertical;
		const pos3 _screen_bottomleft;
	};
}