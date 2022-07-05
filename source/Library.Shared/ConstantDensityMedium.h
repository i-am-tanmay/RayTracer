#pragma once
#include "IRenderObject.h"
#include "Material_RandomScatter.h"

namespace Library
{
	class ConstantDensityMedium final : public IRenderObject
	{
	public:
		ConstantDensityMedium(std::shared_ptr<IRenderObject> container, const color& c, const precision& density) : _container{ container }, _phase_material{ std::make_shared<Material_RandomScatter>(c) }, _negativeinverse_density{ -1 / density } {}
		ConstantDensityMedium(std::shared_ptr<IRenderObject> container, std::shared_ptr<Texture> texture, const precision& density) : _container{ container }, _phase_material{ std::make_shared<Material_RandomScatter>(texture) }, _negativeinverse_density{ -1 / density } {}

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<IRenderObject> _container;
		std::shared_ptr<Material> _phase_material;
		precision _negativeinverse_density{ 0 };
	};
}