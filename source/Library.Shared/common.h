#pragma once

#include <limits>

namespace Library
{
	using precision = double;

	// constants
	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;

	// utility
	inline double degrees_to_radians(double degrees) {
		return degrees * pi / 180.0;
	}
}