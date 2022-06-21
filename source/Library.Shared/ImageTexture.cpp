#include "pch.h"
#include "ImageTexture.h"

namespace Library
{
	ImageTexture::ImageTexture(const char* filename)
	{
		int pixel_bytes = 3;

		_data = stbi_load(filename, &_width, &_height, &pixel_bytes, pixel_bytes);

		if (_data == nullptr) 
		{
			std::cerr << "Could not load texture image file '" << filename << "'.\n";
			_width = _height = 0;
		}

		_byte_width = 3 * _width;
	}

	color ImageTexture::get_color(const precision& u, const precision& v, const pos3&) const
	{
		if (_data == nullptr) return color(1, 0, 1);

		int i = static_cast<int>(clamp(u, 0.0, 1.0) * _width);
		int j = static_cast<int>((1.0 - clamp(v, 0.0, 1.0)) * _height);	// Flip V to image coordinates

		if (i >= _width)  i = _width - 1;
		if (j >= _height) j = _height - 1;

		std::uint8_t* pixel = _data + j * _byte_width + i * 3;

		return color{ color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2] };
	}

	ImageTexture::~ImageTexture()
	{
		delete[] _data;
		_data = nullptr;
	}
}