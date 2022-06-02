#include "pch.h"
#include "Camera.h"

namespace Library
{
	Camera::Camera(pos3 lookfrom, pos3 lookat, precision vertical_fov_degrees, vec3 vertical_up, precision aperture, precision focus_dist)
	{
		precision viewport_height = 2.0 * tan(degrees_to_radians(vertical_fov_degrees) / 2);
		precision viewport_width = aspect_ratio * viewport_height;

		vec3 w = unit_vector(lookfrom - lookat);
		_u = unit_vector(cross(vertical_up, w));
		_v = cross(w, _u);

		_origin = lookfrom;
		_screen_horizontal = focus_dist * viewport_width * _u;
		_screen_vertical = focus_dist * viewport_height * _v;
		_screen_bottomleft = _origin - _screen_horizontal / 2 - _screen_vertical / 2 - focus_dist * w;

		_lensradius = aperture / 2;
	}

	Ray Camera::get_ray(precision u, precision v) const
	{
		vec3 rand_disc = _lensradius * random_in_unit_circle();
		vec3 offset = _u * rand_disc.x() + _v * rand_disc.y();

		return Ray{ _origin + offset, _screen_bottomleft + u * _screen_horizontal + v * _screen_vertical - _origin - offset };
	}
}