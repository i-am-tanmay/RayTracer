#pragma once
#include "IRenderObject.h"

namespace Library
{
	class Translate final : public IRenderObject
	{
	public:
		Translate(std::shared_ptr<IRenderObject> object, vec3 offset) : _object{ object }, _offset{ offset } {}

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<IRenderObject> _object;
		vec3 _offset{ 0,0,0 };
	};
}