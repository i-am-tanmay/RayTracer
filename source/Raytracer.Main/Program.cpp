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
#include "Material_Dielectric.h"

using namespace Library;

int main(int, char**)
{
	// OUTPUT IMAGE
	const std::size_t img_width = 1280;
	const std::size_t img_height = static_cast<int>(img_width / aspect_ratio);

	std::uint8_t* img_rgb = new std::uint8_t[img_width * img_height * 3];
	assert(img_rgb != nullptr);

	// PROPERTIES
	const std::size_t samples_per_pixel = 500;
	const std::size_t bounce_limit = 50;

	// WORLD
	RenderObjectList world;

	// ------------------------------------------------------------ generate random world ------------------------------------------------------
	std::shared_ptr<Material_Lambertian> ground_material = std::make_shared<Material_Lambertian>(color(0.5, 0.5, 0.5));
	world.add(std::make_shared<Sphere>(pos3(0, -1000, 0), 1000, ground_material));

	for (std::int32_t i = -11; i < 11; ++i)
	{
		for (std::int32_t ii = -11; ii < 11; ++ii)
		{
			precision choose_mat = get_random01();
			pos3 center(i + 0.9 * get_random01(), 0.2, ii + 0.9 * get_random01());

			if ((center - pos3(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = std::make_shared<Material_Lambertian>(albedo);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = get_random(0, 0.5);
					sphere_material = std::make_shared<Material_Metal>(albedo, fuzz);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = std::make_shared<Material_Dielectric>(1.5);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	std::shared_ptr<Material_Dielectric> material1 = std::make_shared<Material_Dielectric>(1.5);
	world.add(std::make_shared<Sphere>(pos3(0, 1, 0), 1.0, material1));

	std::shared_ptr<Material_Lambertian> material2 = std::make_shared<Material_Lambertian>(color(0.4, 0.2, 0.1));
	world.add(std::make_shared<Sphere>(pos3(-4, 1, 0), 1.0, material2));

	std::shared_ptr<Material_Metal> material3 = std::make_shared<Material_Metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(std::make_shared<Sphere>(pos3(4, 1, 0), 1.0, material3));
	// // --------------------------------------------------------------------------------------------------------------------------------------

	// RENDER IMAGE
	Camera camera{ pos3 {13,2,3}, pos3{0,0,0}, 20.0, vec3{0,1,0}, 0.1, 10.0 };

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
					precision u = (ii + get_random01()) / (img_width - 1);
					precision v = (i + get_random01()) / (img_height - 1);
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
					precision u = (ii + get_random01()) / (img_width - 1);
					precision v = (i + get_random01()) / (img_height - 1);
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
					precision u = (ii + get_random01()) / (img_width - 1);
					precision v = (i + get_random01()) / (img_height - 1);
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