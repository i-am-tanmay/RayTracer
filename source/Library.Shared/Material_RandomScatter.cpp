#include "pch.h"
#include "Material_RandomScatter.h"
#include "IRenderObject.h"

namespace Library
{
	bool Material_RandomScatter::scatter(const Ray&, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const
	{
		ray_scattered = Ray{ hitinfo.pos, random_in_unit_sphere() };
		attenuation = _albedo->get_color(hitinfo.u, hitinfo.v, hitinfo.pos);
		return true;
	}
}