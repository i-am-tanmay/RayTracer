#pragma once

#include "vec3.h"
#include "Material.h"
#include "Texture.h"

namespace Library
{
	class Material_Light_Diffuse final : public Material
	{
	public:
		Material_Light_Diffuse(std::shared_ptr<Texture> albedo) : _emit_color(albedo) {}
		Material_Light_Diffuse(color c) : _emit_color{ std::make_shared<SolidColor>(c) } {}

		virtual color emitted(const precision& u, const precision& v, const pos3& pos) const override;
		virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const override;

		virtual color denoise_albedo(const precision& u, const precision& v, const pos3& pos) const override { return _emit_color->get_color(u, v, pos); }

	private:
		std::shared_ptr<Texture> _emit_color;
	};
}