#pragma once
#include "IRenderObject.h"

namespace Library
{
	class Rotate_X final : public IRenderObject
	{
	public:
		Rotate_X(std::shared_ptr<IRenderObject> object, const precision& angle);

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<IRenderObject> _object;
		precision sin_angle{ 0 };
		precision cos_angle{ 0 };
	};

	class Rotate_Y final : public IRenderObject
	{
	public:
		Rotate_Y(std::shared_ptr<IRenderObject> object, const precision& angle);

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<IRenderObject> _object;
		precision sin_angle{ 0 };
		precision cos_angle{ 0 };
	};

	class Rotate_Z final : public IRenderObject
	{
	public:
		Rotate_Z(std::shared_ptr<IRenderObject> object, const precision& angle);

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<IRenderObject> _object;
		precision sin_angle{ 0 };
		precision cos_angle{ 0 };
	};
}