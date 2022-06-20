#pragma once

#include <vector>
#include "IRenderObject.h"

namespace Library
{
	class RenderObjectList : public IRenderObject
	{
	public:
		RenderObjectList() = default;
		RenderObjectList(std::initializer_list<std::shared_ptr<IRenderObject>> list);

		void add(std::shared_ptr<IRenderObject> renderobject);

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		bool aabb(AABB& out_box) const override;

		const std::vector<std::shared_ptr<IRenderObject>>& List() const { return _renderobjects; };
		std::size_t Size() const { return _renderobjects.size(); };
		void Clear() { _renderobjects.clear(); };

	private:
		std::vector<std::shared_ptr<IRenderObject>> _renderobjects;
	};
}