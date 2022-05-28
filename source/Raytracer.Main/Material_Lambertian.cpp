#include "pch.h"
#include "Material_Lambertian.h"
#include "IRenderObject.h"

bool Material_Lambertian::scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered, vec3 random_diffuse, bool) const
{
	vec3 scatter_direction = random_diffuse;
	ray_scattered = Ray{ hitinfo.pos, (scatter_direction.is_near_zero()) ? hitinfo.normal : scatter_direction };
	attenuation = albedo;

	return true;
}