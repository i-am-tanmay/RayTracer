#pragma once

#include "IRenderObject.h"
#include "vec3.h"

namespace Library
{
    class Sphere : public IRenderObject {
    public:
        Sphere(pos3 center, precision radius, std::shared_ptr<Material> material) : _center{ center }, _radius{ radius }, _material{ material } {}

        virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& rec) const override;

    private:
        pos3 _center;
        precision _radius;
        std::shared_ptr<Material> _material;
    };
}