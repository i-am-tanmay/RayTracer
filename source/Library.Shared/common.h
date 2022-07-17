#pragma once

#include <limits>
#include <random>

namespace Library
{
	// properties

	using precision = double;
	constexpr precision aspect_ratio = 16.0 / 9.0;

	// constants

	constexpr precision infinity = std::numeric_limits<precision>::infinity();
	constexpr precision pi = 3.1415926535897932385;
	constexpr precision pi_inverse = 1.0 / pi;

	// utility

	inline precision degrees_to_radians(precision degrees)
	{
		return degrees * pi / 180.0;
	}

	inline precision get_random01()
	{
		static std::random_device rnd;
		static std::seed_seq seed{ rnd(), rnd(), rnd(), rnd(), rnd(), rnd(), rnd(), rnd() };
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::minstd_rand generator{ seed };

		return distribution(generator);
	}

	inline precision get_random(const precision& min_included, const precision& max_excluded)
	{
		return ((max_excluded - min_included) * get_random01()) + min_included;
	}

	inline int get_random_int(int min_included, int max_included)
	{
		return static_cast<int>(get_random(min_included, max_included + 1));
	}

	inline precision clamp(const precision& val, const precision& min, const precision& max)
	{
		const precision res = (val < min) ? min : val;
		return (res > max) ? max : res;
	}
}