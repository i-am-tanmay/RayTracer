#include "pch.h"
#include "Material_Metal.h"
#include "IRenderObject.h"

bool Material_Metal::scatter(const Ray& ray, const HitInfo& hitinfo, vec3& attenuation, Ray& ray_scattered, vec3 random_diffuse, bool ishemisphere) const
{
    ray_scattered = Ray{ hitinfo.pos, reflect(unit_vector(ray.direction()), hitinfo.normal) + fuzz * (random_diffuse - ((ishemisphere) ? vec3{0,0,0} : hitinfo.normal)) };
    attenuation = albedo;
    return (dot(ray_scattered.direction(), hitinfo.normal) > 0);
}