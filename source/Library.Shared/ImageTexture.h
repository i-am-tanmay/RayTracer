#pragma once

#pragma warning (push, 0)
#include "stb_image_read.h"
#pragma warning (pop)

#include "Texture.h"

namespace Library
{
	class ImageTexture final : public Texture
	{
    public:
        const precision color_scale = 1.0 / 255.0;

    public:
        ImageTexture() = default;
        ImageTexture(const char* filename);
        ~ImageTexture();

        virtual color get_color(const precision& u, const precision& v, const pos3& pos) const override;

    private:
        std::uint8_t* _data{ nullptr };
        int _width{ 0 };
        int _height{ 0 };
        int _byte_width{ 0 };
    };
}