#include "pch.h"
#include "Material_Light_Diffuse.h"

namespace Library
{
	color Material_Light_Diffuse::emitted(const precision& u, const precision& v, const pos3& pos) const
	{
		return _emit_color->get_color(u, v, pos);
	}

	bool Material_Light_Diffuse::scatter(const Ray&, const HitInfo&, vec3&, Ray&) const
	{
		return false;
	}
}