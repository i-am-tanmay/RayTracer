#pragma once

#include "Ray.h"

struct HitInfo
{
    pos3 hitpos;
    vec3 normal;
    precision ray_t;
};

class IRenderObject
{
public:
	virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& rec) const = 0;
    virtual ~IRenderObject() = default;
};