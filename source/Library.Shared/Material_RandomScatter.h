#pragma once
#include "Material.h"
#include "Texture.h"

namespace Library
{
	class Material_RandomScatter final : public Material
	{
	public:
		Material_RandomScatter(const color& c) : _albedo{ std::make_shared<SolidColor>(c) } {}
		Material_RandomScatter(std::shared_ptr<Texture> albedo) : _albedo{ albedo } {}

		virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const override;

	private:
		std::shared_ptr<Texture> _albedo;
	};
}