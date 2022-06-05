#pragma once
#include "vec3.h"
#include "Material.h"

using namespace Library;

class Material_Dielectric final : public Material
{
public:
	Material_Dielectric(precision index_of_refraction) : _indexofrefraction{ index_of_refraction } {}
	virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const override;

private:
	precision reflectance(precision cosine, precision refraction_ratio) const;

private:
	precision _indexofrefraction;
};