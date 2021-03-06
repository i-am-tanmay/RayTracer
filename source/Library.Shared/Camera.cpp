#include "pch.h"
#include "Camera.h"

namespace Library
{
	Camera::Camera(const pos3& lookfrom, const pos3& lookat, const precision& vertical_fov_degrees, const pos3& vertical_up, const precision& aperture, const precision& focus_dist)
	{
		precision viewport_height = focus_dist * tan(degrees_to_radians(vertical_fov_degrees) / 2);
		precision viewport_width = aspect_ratio * viewport_height;

		vec3 w = unit_vector(lookfrom - lookat);
		_u = unit_vector(cross(vertical_up, w));
		_v = cross(w, _u);

		_origin = lookfrom;
		_screen_horizontal = 2.0 * viewport_width * _u;
		_screen_vertical = 2.0 * viewport_height * _v;
		_screen_bottomleft = _origin - viewport_width * _u - viewport_height * _v - focus_dist * w;

		_lensradius = aperture / 2;
	}

	Ray Camera::get_ray(const precision& u, const precision& v) const
	{
		vec3 rand_disc = _lensradius * random_in_unit_circle();
		vec3 offset = _u * rand_disc.x() + _v * rand_disc.y();

		return Ray{ _origin + offset, _screen_bottomleft + u * _screen_horizontal + v * _screen_vertical - _origin - offset };
	}
}