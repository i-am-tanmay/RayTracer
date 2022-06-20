#include "pch.h"
#include "AABB.h"

namespace Library
{
	bool AABB::hit(const Ray& ray, precision t_min, precision t_max) const
	{
		const vec3& ray_origin = ray.origin();
		const vec3& ray_direction = ray.direction();

		for (std::size_t i = 0; i < 3; ++i)
		{
			precision dir_inverse = 1.0f / ray_direction[i];
			precision t0 = (_min[i] - ray_origin[i]) * dir_inverse;
			precision t1 = (_max[i] - ray_origin[i]) * dir_inverse;
			if (dir_inverse < 0.0f) std::swap(t0, t1);

			t_min = t0 > t_min ? t0 : t_min; // max(t0, t_min)
			t_max = t1 < t_max ? t1 : t_max; // min(t1, t_max)

			if (t_max <= t_min) return false;
		}

		return true;
	}

	std::size_t AABB::longest_axis() const
	{
		precision x = _max[0] - _min[0];
		precision y = _max[1] - _min[1];
		precision z = _max[2] - _min[2];

		if (x >= y && x >= z) return 0;
		else if (y >= x && y >= z) return 1;
		else return 2;
	}

	AABB surrounding_box(const AABB& box0, const AABB& box1)
	{
		return AABB
		{
			pos3 { // min
				box0._min[0] < box1._min[0] ? box0._min[0] : box1._min[0],
				box0._min[1] < box1._min[1] ? box0._min[1] : box1._min[1],
				box0._min[2] < box1._min[2] ? box0._min[2] : box1._min[2],
			},

			pos3 { // max
				box0._max[0] > box1._max[0] ? box0._max[0] : box1._max[0],
				box0._max[1] > box1._max[1] ? box0._max[1] : box1._max[1],
				box0._max[2] > box1._max[2] ? box0._max[2] : box1._max[2],
			}

		};
	}
}