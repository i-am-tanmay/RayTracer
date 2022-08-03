#include "pch.h"
#include "Rotate.h"

namespace Library
{
	Rotate_X::Rotate_X(std::shared_ptr<IRenderObject> object, const precision& angle) : _object{ object }
	{
		precision radians = degrees_to_radians(angle);
		sin_angle = std::sin(radians);
		cos_angle = std::cos(radians);
	}

	bool Rotate_X::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		const vec3& ray_origin = ray.origin();
		const vec3& ray_direction = ray.direction();

		vec3 new_origin = ray.origin();
		vec3 new_direction = ray.direction();

		// y
		new_origin[1] = cos_angle * ray_origin[1] + sin_angle * ray_origin[2];
		new_direction[1] = cos_angle * ray_direction[1] + sin_angle * ray_direction[2];
		// z
		new_origin[2] = -sin_angle * ray_origin[1] + cos_angle * ray_origin[2];
		new_direction[2] = -sin_angle * ray_direction[1] + cos_angle * ray_direction[2];

		Ray ray_rotated{ new_origin, new_direction };
		if (!_object->hit(ray_rotated, t_min, t_max, hitinfo)) return false;

		const vec3& hit_pos = hitinfo.pos;
		const vec3& hit_normal = hitinfo.normal;

		vec3 new_pos = hitinfo.pos;
		vec3 new_normal = hitinfo.normal;

		// y
		new_pos[1] = cos_angle * hit_pos[1] - sin_angle * hit_pos[2];
		new_normal[1] = cos_angle * hit_normal[1] - sin_angle * hit_normal[2];
		// z
		new_pos[2] = sin_angle * hit_pos[1] + cos_angle * hit_pos[2];
		new_normal[2] = sin_angle * hit_normal[1] + cos_angle * hit_normal[2];

		hitinfo.pos = new_pos;
		hitinfo.hit_updatenormal(ray_rotated, new_normal);
		return true;
	}

	bool Rotate_X::aabb(AABB& out_box) const
	{
		AABB out_aabb;
		bool result = _object->aabb(out_aabb);

		vec3 min{ infinity, infinity, infinity };
		vec3 max{ -infinity, -infinity, -infinity };

		for (std::size_t i = 0; i < 2; ++i)
			for (std::size_t ii = 0; ii < 2; ++ii)
				for (std::size_t iii = 0; iii < 2; ++iii)
				{
					precision x = i * out_aabb.max().x() + (1 - i) * out_aabb.min().x();
					precision y = ii * out_aabb.max().y() + (1 - ii) * out_aabb.min().y();
					precision z = iii * out_aabb.max().z() + (1 - iii) * out_aabb.min().z();

					precision y_new = cos_angle * y - sin_angle * z;
					precision z_new = sin_angle * y + cos_angle * z;

					vec3 aabb_new{ x, y_new, z_new };

					min = vec3{ std::min(min.x(), aabb_new.x()),std::min(min.y(), aabb_new.y()) ,std::min(min.z(), aabb_new.z()) };
					max = vec3{ std::max(max.x(), aabb_new.x()),std::max(max.y(), aabb_new.y()) ,std::max(max.z(), aabb_new.z()) };
				}

		out_box = AABB{ min,max };
		return result;
	}

	Library::precision Rotate_X::aabb_area() const
	{
		return _object->aabb_area();
	}

	Rotate_Y::Rotate_Y(std::shared_ptr<IRenderObject> object, const precision& angle) : _object{ object }
	{
		precision radians = degrees_to_radians(angle);
		sin_angle = std::sin(radians);
		cos_angle = std::cos(radians);
	}

	bool Rotate_Y::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		const vec3& ray_origin = ray.origin();
		const vec3& ray_direction = ray.direction();

		vec3 new_origin = ray.origin();
		vec3 new_direction = ray.direction();

		// x
		new_origin[0] = cos_angle * ray_origin[0] - sin_angle * ray_origin[2];
		new_direction[0] = cos_angle * ray_direction[0] - sin_angle * ray_direction[2];
		// z
		new_origin[2] = sin_angle * ray_origin[0] + cos_angle * ray_origin[2];
		new_direction[2] = sin_angle * ray_direction[0] + cos_angle * ray_direction[2];

		Ray ray_rotated{ new_origin, new_direction };
		if (!_object->hit(ray_rotated, t_min, t_max, hitinfo)) return false;

		const vec3& hit_pos = hitinfo.pos;
		const vec3& hit_normal = hitinfo.normal;

		vec3 new_pos = hitinfo.pos;
		vec3 new_normal = hitinfo.normal;

		// x
		new_pos[0] = cos_angle * hit_pos[0] + sin_angle * hit_pos[2];
		new_normal[0] = cos_angle * hit_normal[0] + sin_angle * hit_normal[2];
		// z
		new_pos[2] = -sin_angle * hit_pos[0] + cos_angle * hit_pos[2];
		new_normal[2] = -sin_angle * hit_normal[0] + cos_angle * hit_normal[2];

		hitinfo.pos = new_pos;
		hitinfo.hit_updatenormal(ray_rotated, new_normal);
		return true;
	}

	bool Rotate_Y::aabb(AABB& out_box) const
	{
		AABB out_aabb;
		bool result = _object->aabb(out_aabb);

		vec3 min{ infinity, infinity, infinity };
		vec3 max{ -infinity, -infinity, -infinity };

		for (std::size_t i = 0; i < 2; ++i)
			for (std::size_t ii = 0; ii < 2; ++ii)
				for (std::size_t iii = 0; iii < 2; ++iii)
				{
					precision x = i * out_aabb.max().x() + (1 - i) * out_aabb.min().x();
					precision y = ii * out_aabb.max().y() + (1 - ii) * out_aabb.min().y();
					precision z = iii * out_aabb.max().z() + (1 - iii) * out_aabb.min().z();

					precision x_new = cos_angle * x + sin_angle * z;
					precision z_new = -sin_angle * x + cos_angle * z;

					vec3 aabb_new{ x_new, y, z_new };

					min = vec3{ std::min(min.x(), aabb_new.x()),std::min(min.y(), aabb_new.y()) ,std::min(min.z(), aabb_new.z()) };
					max = vec3{ std::max(max.x(), aabb_new.x()),std::max(max.y(), aabb_new.y()) ,std::max(max.z(), aabb_new.z()) };
				}

		out_box = AABB{ min,max };
		return result;
	}

	Library::precision Rotate_Y::aabb_area() const
	{
		return _object->aabb_area();
	}

	Rotate_Z::Rotate_Z(std::shared_ptr<IRenderObject> object, const precision& angle) : _object{ object }
	{
		precision radians = degrees_to_radians(angle);
		sin_angle = std::sin(radians);
		cos_angle = std::cos(radians);
	}

	bool Rotate_Z::hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const
	{
		const vec3& ray_origin = ray.origin();
		const vec3& ray_direction = ray.direction();

		vec3 new_origin = ray.origin();
		vec3 new_direction = ray.direction();

		// x
		new_origin[0] = cos_angle * ray_origin[0] + sin_angle * ray_origin[1];
		new_direction[0] = cos_angle * ray_direction[0] + sin_angle * ray_direction[1];
		// y
		new_origin[1] = -sin_angle * ray_origin[0] + cos_angle * ray_origin[1];
		new_direction[1] = -sin_angle * ray_direction[0] + cos_angle * ray_direction[1];

		Ray ray_rotated{ new_origin, new_direction };
		if (!_object->hit(ray_rotated, t_min, t_max, hitinfo)) return false;

		const vec3& hit_pos = hitinfo.pos;
		const vec3& hit_normal = hitinfo.normal;

		vec3 new_pos = hitinfo.pos;
		vec3 new_normal = hitinfo.normal;

		// x
		new_pos[0] = cos_angle * hit_pos[0] - sin_angle * hit_pos[1];
		new_normal[0] = cos_angle * hit_normal[0] - sin_angle * hit_normal[1];
		// y
		new_pos[1] = sin_angle * hit_pos[0] + cos_angle * hit_pos[1];
		new_normal[1] = sin_angle * hit_normal[0] + cos_angle * hit_normal[1];

		hitinfo.pos = new_pos;
		hitinfo.hit_updatenormal(ray_rotated, new_normal);
		return true;
	}

	bool Rotate_Z::aabb(AABB& out_box) const
	{
		AABB out_aabb;
		bool result = _object->aabb(out_aabb);

		vec3 min{ infinity, infinity, infinity };
		vec3 max{ -infinity, -infinity, -infinity };

		for (std::size_t i = 0; i < 2; ++i)
			for (std::size_t ii = 0; ii < 2; ++ii)
				for (std::size_t iii = 0; iii < 2; ++iii)
				{
					precision x = i * out_aabb.max().x() + (1 - i) * out_aabb.min().x();
					precision y = ii * out_aabb.max().y() + (1 - ii) * out_aabb.min().y();
					precision z = iii * out_aabb.max().z() + (1 - iii) * out_aabb.min().z();

					precision x_new = cos_angle * x - sin_angle * y;
					precision y_new = sin_angle * x + cos_angle * y;

					vec3 aabb_new{ x_new, y_new, z };

					min = vec3{ std::min(min.x(), aabb_new.x()),std::min(min.y(), aabb_new.y()) ,std::min(min.z(), aabb_new.z()) };
					max = vec3{ std::max(max.x(), aabb_new.x()),std::max(max.y(), aabb_new.y()) ,std::max(max.z(), aabb_new.z()) };
				}

		out_box = AABB{ min,max };
		return result;
	}

	Library::precision Rotate_Z::aabb_area() const
	{
		return _object->aabb_area();
	}
}