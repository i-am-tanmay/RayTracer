#pragma once
#include "vec3.h"
#include "Material.h"
#include "Texture.h"

namespace Library
{
	class Material_Metal final : public Material
	{
	public:
		Material_Metal(std::shared_ptr<Texture> texture, precision fuzziness = 1.0) :albedo{ texture }, fuzz{ (fuzziness < 1) ? fuzziness : 1 } {}
		Material_Metal(const color& color_albedo = color{ 1,0,1 }, precision fuzziness = 1.0) : albedo{ std::make_shared<SolidColor>(color_albedo) }, fuzz{ (fuzziness < 1) ? fuzziness : 1 } {}
		virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const override;

	public:
		std::shared_ptr<Texture> albedo;
		precision fuzz;
	};
}