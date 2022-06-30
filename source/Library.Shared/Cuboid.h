#pragma once
#include "Rect.h"

namespace Library
{
	class Cuboid final : public IRenderObject
	{
	public:
		Cuboid(const vec3& center, precision x, precision y, precision z, std::shared_ptr<Material> texture);

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		vec3 _center{ 0,0,0 };
		vec3 _size{ 0,0,0 };
		std::vector<std::shared_ptr<IRenderObject>> _sides;
	};
}