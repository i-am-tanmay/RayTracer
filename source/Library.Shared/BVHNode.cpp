#include "pch.h"
#include "BVHNode.h"
#include <algorithm>

namespace Library
{
	BVHNode::BVHNode(const std::vector<std::shared_ptr<IRenderObject>>& list_objects)
	{
		if (list_objects.empty()) throw std::runtime_error("BVH: Empty Object List");

		std::size_t count = list_objects.size();

		// If only 1 object
		if (count == 1)
		{
			_left = _right = list_objects[0];
		}
		// If 2 objects
		else if (count == 2)
		{
			_left = list_objects[0];
			_right = list_objects[1];
		}
		// Divide objects according to SurfaceArea-Heuristic (SAH)
		else
		{
			std::vector<std::shared_ptr<IRenderObject>> list_objects_sort = list_objects;

			// Get BOX Surrounding Everything to get Longest Axis
			AABB main_box;
			if (!list_objects_sort[0]->aabb(main_box)) std::cerr << "aabb failed\n";
			for (std::size_t i = 1; i < count; ++i)
			{
				AABB new_box;
				if (!list_objects_sort[i]->aabb(new_box)) std::cerr << "aabb failed\n";
				main_box = surrounding_box(new_box, main_box);
			}

			std::size_t axis = main_box.longest_axis();
			auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

			std::sort(list_objects_sort.begin(), list_objects_sort.end(), comparator);

			precision min_SAH = std::numeric_limits<precision>::max();
			std::size_t mid;
			for (std::size_t i = 0; i < count - 1; ++i)
			{
				precision SAH = i * list_objects_sort[i]->aabb_area() + (count - 1 - i) * list_objects_sort[count - 1 - i]->aabb_area();

				if (SAH < min_SAH)
				{
					mid = i;
					min_SAH = SAH;
				}
			}

			std::vector<std::shared_ptr<IRenderObject>> list_objects_divided;
			list_objects_divided.reserve(std::max(mid + 1, count - mid - 1));

			for (std::size_t i = 0; i <= mid; ++i) list_objects_divided.push_back(list_objects_sort[i]);
			_left = std::make_shared<BVHNode>(list_objects_divided);

			list_objects_divided.clear();

			for (std::size_t i = mid + 1; i < count; ++i) list_objects_divided.push_back(list_objects_sort[i]);
			_right = std::make_shared<BVHNode>(list_objects_divided);
		}

		AABB box_left, box_right;
		if (!_left->aabb(box_left) || !_right->aabb(box_right)) std::cerr << "aabb failed\n";
		_box = surrounding_box(box_left, box_right);
	}

	bool box_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b, std::size_t axis)
	{
		AABB box_a;
		AABB box_b;
		if (!a->aabb(box_a) || !b->aabb(box_b)) std::cerr << "aabb failed\n";
		return box_a.min()[axis] < box_b.min()[axis];
	}

	bool box_x_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b) { return box_compare(a, b, 0); }
	bool box_y_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b) { return box_compare(a, b, 1); }
	bool box_z_compare(const std::shared_ptr<IRenderObject>& a, const std::shared_ptr<IRenderObject>& b) { return box_compare(a, b, 2); }

	bool BVHNode::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		if (!_box.hit(ray, t_min, t_max)) return false;

		bool hit_left = _left->hit(ray, t_min, t_max, hitinfo);
		bool hit_right = _right->hit(ray, t_min, hit_left ? hitinfo.ray_t : t_max, hitinfo);

		return hit_left || hit_right;
	}

	bool BVHNode::aabb(AABB& out_box) const
	{
		out_box = _box;
		return true;
	}

	precision BVHNode::aabb_area() const
	{
		return _left->aabb_area() + _right->aabb_area();
	}
}