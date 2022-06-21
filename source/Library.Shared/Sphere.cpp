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
        vec3 out_normal = (hitinfo.pos - _center) / _radius;
        hitinfo.hit_updatenormal(ray, out_normal);
        get_uv(out_normal, hitinfo.u, hitinfo.v);
        hitinfo.material = _material;

        return true;
    }

    void Sphere::get_uv(const pos3& pos, precision& u, precision& v)
    {
        auto theta = acos(-pos.y());
        auto phi = atan2(-pos.z(), pos.x()) + pi;

        u = phi * pi_inverse * 0.5;
        v = theta * pi_inverse;
    }

	bool Sphere::aabb(AABB& out_box) const
	{
        out_box = AABB
        {
            vec3{_center[0] - _radius, _center[1] - _radius,_center[2] - _radius},
            vec3{_center[0] + _radius, _center[1] + _radius,_center[2] + _radius}
        };

		return true;
	}

	precision Sphere::aabb_area() const
	{
        return 4 * pi * _radius * _radius;
	}
}