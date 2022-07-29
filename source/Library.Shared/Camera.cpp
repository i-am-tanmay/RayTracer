#include "pch.h"
#include "Camera.h"

namespace Library
{
	Camera::Camera(const pos3& pos, const precision& vertical_fov_degrees, const precision& aperture, const precision& focus_dist) :
		_focusdist{ focus_dist }, _lensradius{ aperture / 2 }
	{
		_viewport_height = focus_dist * tan(degrees_to_radians(vertical_fov_degrees) / 2);
		_viewport_width = aspect_ratio * _viewport_height;

		_forward = vec3{ 0,0,-1 };
		_right = unit_vector(cross(vertical_up, _forward));
		_up = cross(_forward, _right);

		_position = pos;
		_screen_horizontal = 2.0 * _viewport_width * _right;
		_screen_vertical = 2.0 * _viewport_height * _up;
		_screen_bottomleft = _position - _viewport_width * _right - _viewport_height * _up - focus_dist * _forward;
	}

	Ray Camera::get_ray(const precision& u, const precision& v) const
	{
		vec3 rand_disc = _lensradius * random_in_unit_circle();
		vec3 offset = _right * rand_disc.x() + _up * rand_disc.y();

		return Ray{ _position + offset, _screen_bottomleft + u * _screen_horizontal + v * _screen_vertical - _position - offset };
	}

	void Camera::Move(bool forward, bool back, bool left, bool right)
	{
		if (forward || back || left || right)
		{
			pos3 offset = ((back * _movespeed - forward * _movespeed) * _forward) + ((right * _movespeed - left * _movespeed) * _right);

			_position += offset;
			_screen_bottomleft += offset;

			_updated = true;
		}
	}

	void Camera::Rotate(const precision& x, const precision& y)
	{
		if (std::abs(x) < 0.1 && std::abs(y) < 0.1) return;

		_yaw += x * _rotspeed;
		_pitch += y * _rotspeed;

		precision cospitch = std::cos(_pitch);
		vec3 forward
		{
			std::cos(_yaw) * cospitch,
			std::sin(_pitch),
			std::sin(_yaw) * cospitch
		};

		_forward = unit_vector(forward);
		_right = unit_vector(cross(vertical_up, _forward));
		_up = cross(_forward, _right);

		_screen_horizontal = 2.0 * _viewport_width * _right;
		_screen_vertical = 2.0 * _viewport_height * _up;
		_screen_bottomleft = _position - _viewport_width * _right - _viewport_height * _up - _focusdist * _forward;

		_updated = true;
	}

	bool Camera::WasUpdated()
	{
		bool updated = _updated;
		_updated = false;
		return updated;
	}
}