#pragma once
#include "common.h"
#include "vec3.h"
#include "Ray.h"

namespace Library
{
	class AABB final
	{
	public:
		AABB() = default;
		AABB(const pos3& bound_min, const pos3& bound_max) : _min{ bound_min }, _max{ bound_max } {}

		inline pos3 min() const { return _min; }
		inline pos3 max() const { return _max; }

		bool hit(const Ray& r, precision t_min, precision t_max) const;

		std::size_t longest_axis() const;

		friend AABB surrounding_box(const AABB& box0, const AABB& box1);

	private:
		pos3 _min{ 0,0,0 };
		pos3 _max{ 0,0,0 };
	};

	AABB surrounding_box(const AABB& box0, const AABB& box1);
}