#pragma once

#include "IRenderObject.h"

namespace Library
{
	class Rect_XY final : public IRenderObject
	{
	public:
		Rect_XY() = default;
		Rect_XY(const precision& xmin, const precision& xmax, const precision& ymin, const precision& ymax, const precision& z, std::shared_ptr<Material> mat) :
			_x_inverse{ 1.0 / (xmax - xmin) }, _xmin{ xmin }, _xmax{ xmax }, _y_inverse{ 1.0 / (ymax - ymin) }, _ymin{ ymin }, _ymax{ ymax }, _z{ z }, _mat{ mat } {}

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<Material> _mat;
		precision _xmin{ 0 };
		precision _xmax{ 0 };
		precision _ymin{ 0 };
		precision _ymax{ 0 };
		precision _z{ 0 };

		precision _x_inverse{ 0 };
		precision _y_inverse{ 0 };
	};

	class Rect_XZ final : public IRenderObject
	{
	public:
		Rect_XZ() = default;
		Rect_XZ(const precision& xmin, const precision& xmax, const precision& zmin, const precision& zmax, const precision& y, std::shared_ptr<Material> mat) :
			_x_inverse{ 1.0 / (xmax - xmin) }, _xmin{ xmin }, _xmax{ xmax }, _z_inverse{ 1.0 / (zmax - zmin) }, _zmin{ zmin }, _zmax{ zmax }, _y{ y }, _mat{ mat } {}

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<Material> _mat;
		precision _xmin{ 0 };
		precision _xmax{ 0 };
		precision _zmin{ 0 };
		precision _zmax{ 0 };
		precision _y{ 0 };

		precision _x_inverse{ 0 };
		precision _z_inverse{ 0 };
	};

	class Rect_YZ final : public IRenderObject
	{
	public:
		Rect_YZ() = default;
		Rect_YZ(const precision& ymin, const precision& ymax, const precision& zmin, const precision& zmax, const precision& x, std::shared_ptr<Material> mat) :
			_y_inverse{ 1.0 / (ymax - ymin) }, _ymin{ ymin }, _ymax{ ymax }, _z_inverse{ 1.0 / (zmax - zmin) }, _zmin{ zmin }, _zmax{ zmax }, _x{ x }, _mat{ mat } {}

		virtual bool hit(const Ray& ray, const precision& t_min, const precision& t_max, HitInfo& hitinfo) const override;
		virtual bool aabb(AABB& out_box) const override;
		virtual precision aabb_area() const override;

	private:
		std::shared_ptr<Material> _mat;
		precision _ymin{ 0 };
		precision _ymax{ 0 };
		precision _zmin{ 0 };
		precision _zmax{ 0 };
		precision _x{ 0 };

		precision _y_inverse{ 0 };
		precision _z_inverse{ 0 };
	};
}