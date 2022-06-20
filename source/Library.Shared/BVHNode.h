#pragma once

#include "IRenderObject.h"

namespace Library
{
	class BVHNode final : public IRenderObject
	{
	public:
		BVHNode(const std::vector<std::shared_ptr<IRenderObject>>& list_objects);

		bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		bool aabb(AABB& out_box) const override;

		precision aabb_area() const override;

	private:
		std::shared_ptr<IRenderObject> _left;
		std::shared_ptr<IRenderObject> _right;
		AABB _box;
	};

	bool box_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b, std::size_t axis);
	bool box_x_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b);
	bool box_y_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b);
	bool box_z_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b);
}