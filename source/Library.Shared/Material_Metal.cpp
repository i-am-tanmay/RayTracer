#include "pch.h"
#include "Material_Metal.h"
#include "IRenderObject.h"

namespace Library
{
    bool Material_Metal::scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered) const
    {
        ray_scattered = Ray{ hitinfo.pos, reflect(unit_vector(ray.direction()), hitinfo.normal) + fuzz * random_in_unit_sphere() };
        attenuation = albedo->get_color(hitinfo.u, hitinfo.v, hitinfo.pos);
        return (dot(ray_scattered.direction(), hitinfo.normal) > 0);
    }
}