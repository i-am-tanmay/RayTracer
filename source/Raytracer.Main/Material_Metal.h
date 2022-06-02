#pragma once
#include "vec3.h"
#include "Material.h"

using namespace Library;

class Material_Metal final : public Material
{
public:
	Material_Metal(color color_albedo = color{ 1,0,1 }, precision fuzziness = 1.0) :albedo{ color_albedo }, fuzz{ (fuzziness<1)?fuzziness:1 } {}
	virtual bool scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered, vec3 random_diffuse, bool ishemisphere = false) const override;

public:
	color albedo;
	precision fuzz;
};