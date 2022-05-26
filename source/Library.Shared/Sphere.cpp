#include "pch.h"
#include "Sphere.h"

namespace Library
{
    bool Sphere::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
    {
        vec3 origin_minus_center = ray.origin() - _center;

        // Solving Quadratic Equation of Sphere

        /* UNOPTIMIZED ORIGINAL EQUATION SOLVE
        precision a = dot(r.direction, r.direction);
        precision b = 2 * dot(origin_minus_center, r.direction);
        precision c = dot(origin_minus_center, origin_minus_center) - radius * radius;
        precision discriminant = b * b - 4 * a * c;*/

        precision a = ray.direction().length_squared();
        precision half_b = dot(origin_minus_center, ray.direction());
        precision c = origin_minus_center.length_squared() - _radius * _radius;
        precision discriminant = half_b * half_b - a * c;

        if (discriminant < 0) return false;

        precision sqrt = std::sqrt(discriminant);

        // Try both roots starting with nearest
        precision root = (-half_b - sqrt) / a;
        if (root < t_min || root > t_max)
        {
            root = (-half_b + sqrt) / a;
            if (root < t_min || root > t_max) return false;
        }

        hitinfo.ray_t = root;
        hitinfo.pos = ray.at(hitinfo.ray_t);
        hitinfo.hit_updatenormal(ray, ((hitinfo.pos - _center) / _radius));

        return true;
    }
}