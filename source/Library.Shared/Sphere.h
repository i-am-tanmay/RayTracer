#pragma once

#include "IRenderObject.h"
#include "vec3.h"

namespace Library
{
    class Sphere final : public IRenderObject
    {
    public:
        Sphere(const pos3& center, const precision& radius, std::shared_ptr<Material> material) : _center{ center }, _radius{ radius }, _material{ material } {}

        virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
        virtual bool aabb(AABB& out_box) const override;
        virtual precision aabb_area() const override;

        static void get_uv(const pos3& pos, precision& u, precision& v);

    private:
        pos3 _center;
        precision _radius;
        std::shared_ptr<Material> _material;
    };
}