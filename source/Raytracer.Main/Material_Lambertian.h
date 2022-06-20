#pragma once
#include "vec3.h"
#include "Material.h"
#include "Texture.h"

using namespace Library;

class Material_Lambertian final : public Material
{
public:
	Material_Lambertian(std::shared_ptr<Texture> texture) : albedo{ texture } {}
	Material_Lambertian(const color& color_albedo = color{ 1,0,1 }) : albedo{ std::make_shared<SolidColor>(color_albedo) } {}
	virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const override;

public:
	std::shared_ptr<Texture> albedo;
};