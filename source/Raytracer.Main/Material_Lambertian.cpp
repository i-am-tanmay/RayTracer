#include "pch.h"
#include "Material_Lambertian.h"
#include "IRenderObject.h"

bool Material_Lambertian::scatter(const Ray&, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const
{
	vec3 scatter_direction = hitinfo.normal + random_unit_vector();
	ray_scattered = Ray{ hitinfo.pos, (scatter_direction.is_near_zero()) ? hitinfo.normal : scatter_direction };
	attenuation = albedo->get_color(hitinfo.u, hitinfo.v, hitinfo.pos);

	return true;
}