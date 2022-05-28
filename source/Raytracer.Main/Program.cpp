#include "pch.h"
#include <iostream>
#include <fstream>
#include <assert.h>

#pragma warning(push)
#pragma warning(disable : 4996 26819 6385 6386 26451)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma warning(pop)

#include "common.h"

#include "vec3.h"
#include "helper.h"
#include "Ray.h"

#include "Camera.h"

#include "RenderObjectList.h"
#include "Sphere.h"

#include "Material_Lambertian.h"
#include "Material_Metal.h"

using namespace Library;

int main(int, char**)
{
	// OUTPUT IMAGE
	const std::size_t img_width = 1280;
	const std::size_t img_height = static_cast<int>(img_width / aspect_ratio);

	std::uint8_t* img_rgb = new std::uint8_t[img_width * img_height * 3];
	assert(img_rgb != nullptr);

	// PROPERTIES
	const std::size_t samples_per_pixel = 100;
	const std::size_t bounce_limit = 50;

	// WORLD
	RenderObjectList world;

	std::shared_ptr<Material_Lambertian> material_ground = std::make_shared<Material_Lambertian>(color(0.8, 0.8, 0.0));
	std::shared_ptr<Material_Lambertian> material_center = std::make_shared<Material_Lambertian>(color(0.7, 0.3, 0.3));
	std::shared_ptr<Material_Metal> material_left = std::make_shared<Material_Metal>(color(0.8, 0.8, 0.8), 0.3);
	std::shared_ptr<Material_Metal> material_right = std::make_shared<Material_Metal>(color(0.8, 0.6, 0.2));

	world.add(std::make_shared<Sphere>(pos3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(std::make_shared<Sphere>(pos3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(std::make_shared<Sphere>(pos3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(std::make_shared<Sphere>(pos3(1.0, 0.0, -1.0), 0.5, material_right));

	// RENDER IMAGE
	Camera camera{ 2.0, 1.0 };

	// cos3 distribution
	{
		for (std::size_t i = 0; i < img_height; ++i)
		{
			std::cout << "\rScanlines remaining: " << img_height - i - 1 << ' ' << std::flush;
			for (std::size_t ii = 0; ii < img_width; ++ii)
			{
				color pixel_color{ 0,0,0 };

				for (std::size_t sample = 0; sample < samples_per_pixel; ++sample)
				{
					precision u = (ii + get_random()) / (img_width - 1);
					precision v = (i + get_random()) / (img_height - 1);
					pixel_color += ray_color_cos3(camera.get_ray(u, v), world, bounce_limit);
				}

				write_color(&img_rgb[(i * img_width + ii) * 3], pixel_color, samples_per_pixel);
			}
		}

		std::cout << "\ntime to write to file xo\n";
		stbi_flip_vertically_on_write(true);
		if (stbi_write_png("../../outputimage_cos3.png", img_width, img_height, 3, img_rgb, img_width * 3) == 0) std::cout << "couldn't write to PNG lolol";
		else std::cout << "fin.\n";
	}

	// cos distribution
	{
		for (std::size_t i = 0; i < img_height; ++i)
		{
			std::cout << "\rScanlines remaining: " << img_height - i - 1 << ' ' << std::flush;
			for (std::size_t ii = 0; ii < img_width; ++ii)
			{
				color pixel_color{ 0,0,0 };

				for (std::size_t sample = 0; sample < samples_per_pixel; ++sample)
				{
					precision u = (ii + get_random()) / (img_width - 1);
					precision v = (i + get_random()) / (img_height - 1);
					pixel_color += ray_color_cos(camera.get_ray(u, v), world, bounce_limit);
				}

				write_color(&img_rgb[(i * img_width + ii) * 3], pixel_color, samples_per_pixel);
			}
		}

		std::cout << "\ntime to write to file xo\n";
		stbi_flip_vertically_on_write(true);
		if (stbi_write_png("../../outputimage_cos.png", img_width, img_height, 3, img_rgb, img_width * 3) == 0) std::cout << "couldn't write to PNG lolol";
		else std::cout << "fin.\n";
	}

	// hemisphere distribution
	{
		for (std::size_t i = 0; i < img_height; ++i)
		{
			std::cout << "\rScanlines remaining: " << img_height - i - 1 << ' ' << std::flush;
			for (std::size_t ii = 0; ii < img_width; ++ii)
			{
				color pixel_color{ 0,0,0 };

				for (std::size_t sample = 0; sample < samples_per_pixel; ++sample)
				{
					precision u = (ii + get_random()) / (img_width - 1);
					precision v = (i + get_random()) / (img_height - 1);
					pixel_color += ray_color_hemispherical(camera.get_ray(u, v), world, bounce_limit);
				}

				write_color(&img_rgb[(i * img_width + ii) * 3], pixel_color, samples_per_pixel);
			}
		}

		std::cout << "\ntime to write to file xo\n";
		stbi_flip_vertically_on_write(true);
		if (stbi_write_png("../../outputimage_hemi.png", img_width, img_height, 3, img_rgb, img_width * 3) == 0) std::cout << "couldn't write to PNG lolol";
		else std::cout << "fin.\n";
	}


	// CLEANUP
	delete[] img_rgb;

	return 0;
}