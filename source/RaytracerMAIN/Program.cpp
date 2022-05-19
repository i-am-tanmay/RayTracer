#include "pch.h"
#include <iostream>
#include <fstream>
#include <assert.h>

#pragma warning(push)
#pragma warning(disable : 4996 26819 6385 6386 26451)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma warning(pop)

#include "vec3.h"
#include "color.h"

using vec3d = vec3<double>;
using color = vec3d;
using pos3 = vec3d;

int main(int, char**)
{
	const std::size_t img_width = 256;
	const std::size_t img_height = 256;

	std::uint8_t* img_rgb = reinterpret_cast<std::uint8_t*>(std::malloc(img_width * img_height * 3));
	assert(img_rgb != nullptr);

	for (std::size_t i = 0; i < img_height; ++i)
	{
		std::cout << "\rScanlines remaining: " << img_height - i - 1 << ' ' << std::flush;
		for (std::size_t ii = 0; ii < img_width; ++ii)
		{
			double r = static_cast<double>(i) / (img_height - 1);
			double g = static_cast<double>(ii) / (img_width - 1);
			double b = 1.0 - std::max(r, g);
			assert(r >= 0); assert(r <= 1);
			assert(g >= 0); assert(g <= 1);
			assert(b >= 0); assert(b <= 1);

			write_color(&img_rgb[(i * img_width + ii) * 3], vec3d{ r,g,b });
		}
	}

	std::cout << "\ntime to write to file xo\n";
	if (stbi_write_png("../../outputimage.png", img_width, img_height, 3, img_rgb, img_width * 3) == 0) throw std::runtime_error("couldn't write to PNG lolol");
	std::cout << "fin.\n";
	free(img_rgb);

	return 0;
}