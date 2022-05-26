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

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& rec) const override;

		void clear();

	public:
		std::vector<std::shared_ptr<IRenderObject>> _renderobjects;
	};
}