#include "pch.h"
#include "RenderObjectList.h"

namespace Library
{
	RenderObjectList::RenderObjectList(std::initializer_list<std::shared_ptr<IRenderObject>> list) : _renderobjects{ list }
	{
	}

	void RenderObjectList::add(std::shared_ptr<IRenderObject> renderobject)
	{
		_renderobjects.push_back(renderobject);
	}

	bool RenderObjectList::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		bool __hit{ false };

		HitInfo __hitinfo;
		precision mindist{ t_max };

		for (std::size_t i = 0; i < _renderobjects.size(); ++i)
		{
			if (_renderobjects[i]->hit(ray, t_min, mindist, __hitinfo))
			{
				__hit = true;
				mindist = __hitinfo.ray_t;
				hitinfo = __hitinfo;
			}
		}

		return __hit;
	}

	bool RenderObjectList::aabb(AABB& out_box) const
	{
		if (_renderobjects.empty()) return false;

		AABB temp_box;

		// first box init
		if (!_renderobjects[0]->aabb(temp_box)) return false;
		out_box = temp_box;

		for (std::size_t i = 1; i < _renderobjects.size(); ++i)
		{
			if (!_renderobjects[i]->aabb(temp_box)) return false;
			out_box = surrounding_box(out_box, temp_box);
		}

		return true;
	}
}