#pragma once
#include "vec3.h"
#include "Material.h"

using namespace Library;

class Material_Lambertian : public Material
{
public:
	Material_Lambertian(color color_albedo = color{ 1,0,1 }) :albedo{ color_albedo } {}
	virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered, vec3 random_diffuse, bool ishemisphere = false) const override;

public:
	color albedo;
};