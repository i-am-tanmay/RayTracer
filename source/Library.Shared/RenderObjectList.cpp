#include "pch.h"
#include "RenderObjectList.h"

namespace Library
{
	RenderObjectList::RenderObjectList(std::initializer_list<std::shared_ptr<IRenderObject>> list) : _renderobjects{ list }
	{
	}

	void RenderObjectList::clear()
	{
		_renderobjects.clear();
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
}