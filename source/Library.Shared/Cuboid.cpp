#include "pch.h"
#include "Cuboid.h"

#include <assert.h>

namespace Library
{
	Cuboid::Cuboid(const vec3& center, precision x, precision y, precision z, std::shared_ptr<Material> texture)
	{
		assert(x > 0); assert(y > 0); assert(z > 0);
		x /= 2; y /= 2; z /= 2;
		_center = center;
		_size = vec3{ x,y,z };

		_sides.push_back(std::make_shared<Rect_XY>(center.x() - x, center.x() + x, center.y() - y, center.y() + y, center.z() - z, texture));
		_sides.push_back(std::make_shared<Rect_XY>(center.x() - x, center.x() + x, center.y() - y, center.y() + y, center.z() + z, texture));

		_sides.push_back(std::make_shared<Rect_XZ>(center.x() - x, center.x() + x, center.z() - z, center.z() + z, center.y() - y, texture));
		_sides.push_back(std::make_shared<Rect_XZ>(center.x() - x, center.x() + x, center.z() - z, center.z() + z, center.y() + y, texture));

		_sides.push_back(std::make_shared<Rect_YZ>(center.y() - y, center.y() + y, center.z() - z, center.z() + z, center.x() - x, texture));
		_sides.push_back(std::make_shared<Rect_YZ>(center.y() - y, center.y() + y, center.z() - z, center.z() + z, center.x() + x, texture));
	}

	bool Cuboid::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		HitInfo tempinfo;
		bool hit{ false };
		precision min_dist{ t_max };

		for (std::size_t i = 0; i < 6; ++i)
		{
			if (_sides[i]->hit(ray, t_min, min_dist, tempinfo))
			{
				hit = true;
				min_dist = tempinfo.ray_t;
				hitinfo = tempinfo;
			}
		}

		return hit;
	}

	bool Cuboid::aabb(AABB& out_box) const
	{
		out_box = AABB
		{
			vec3{_center - _size},
			vec3{_center + _size}
		};
		return true;
	}

	Library::precision Cuboid::aabb_area() const
	{
		return 2 * (_size.x() * _size.y() + _size.y() * _size.z() + _size.z() * _size.x());
	}
}