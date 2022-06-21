#include "pch.h"
#include "Material_Dielectric.h"
#include "IRenderObject.h"

namespace Library
{
	bool Material_Dielectric::scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const
	{
		attenuation = color(1.0, 1.0, 1.0);
		double refraction_ratio = hitinfo.isFrontFace ? (1.0 / _indexofrefraction) : _indexofrefraction;

		vec3 unit_direction = unit_vector(ray.direction());

		precision cos_theta = std::fmin(dot(-unit_direction, hitinfo.normal), 1.0);
		precision sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
		ray_scattered = Ray{ hitinfo.pos,
			vec3{
				((refraction_ratio * sin_theta > 1.0) || reflectance(cos_theta, refraction_ratio) > get_random01()) ?
				reflect(unit_direction,hitinfo.normal) :
				refract(unit_direction,hitinfo.normal, refraction_ratio) }
		};

		return true;
	}

	precision Material_Dielectric::reflectance(precision cosine, precision refraction_ratio) const
	{
		// Schlick's Approximation for reflectance
		precision r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
		r0 = r0 * r0;
		return r0 + (1 - r0) * std::pow((1 - cosine), 5);
	}
}