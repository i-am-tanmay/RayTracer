#include "pch.h"
#include "Camera.h"

namespace Library
{
	Camera::Camera(precision screen_height, precision focal_length, pos3 origin) :
		_origin{ origin },
		_screen_horizontal{ aspect_ratio * screen_height, 0, 0 },
		_screen_vertical{ 0, screen_height, 0 },
		_screen_bottomleft{ origin.x() - (aspect_ratio * screen_height) / 2, origin.y() - screen_height / 2, origin.z() - focal_length }
	{
	}

	Ray Camera::get_ray(precision u, precision v) const
	{
		return Ray{ _origin, _screen_bottomleft + u * _screen_horizontal + v * _screen_vertical - _origin };
	}
}