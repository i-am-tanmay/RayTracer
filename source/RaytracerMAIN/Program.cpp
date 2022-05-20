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
#include "ray.h"

int main(int, char**)
{
	// OUTPUT IMAGE
	const precision aspect_ratio = 16.0 / 9.0;
	const std::size_t img_width = 1920;
	const std::size_t img_height = static_cast<int>(img_width / aspect_ratio);

	std::uint8_t* img_rgb = new std::uint8_t[img_width * img_height * 3];
	assert(img_rgb != nullptr);

	//CAMERA
	const precision viewport_height = 2.0;
	const precision viewport_width = aspect_ratio * viewport_height;
	const precision focal_length = 1.0;

	const vec3 cam_origin{ 0, 0, 0 };
	const vec3 horizontal{ viewport_width, 0, 0 };
	const vec3 vertical{ 0, viewport_height, 0 };
	const vec3 lower_left_corner = cam_origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	// RENDER IMAGE
	for (std::size_t i = 0; i < img_height; ++i)
	{
		std::cout << "\rScanlines remaining: " << img_height - i - 1 << ' ' << std::flush;
		for (std::size_t ii = 0; ii < img_width; ++ii)
		{
			double r = static_cast<double>(i) / (img_height - 1); assert(r >= 0); assert(r <= 1);
			double g = static_cast<double>(ii) / (img_width - 1); assert(g >= 0); assert(g <= 1);
			double b = 1.0 - std::max(r, g); assert(b >= 0); assert(b <= 1);

			write_color(&img_rgb[(i * img_width + ii) * 3], vec3{ r,g,b });
		}
	}

	std::cout << "\ntime to write to file xo\n";
	if (stbi_write_png("../../outputimage.png", img_width, img_height, 3, img_rgb, img_width * 3) == 0) throw std::runtime_error("couldn't write to PNG lolol");
	std::cout << "fin.\n";

	// CLEANUP
	delete[] img_rgb;

	return 0;
}