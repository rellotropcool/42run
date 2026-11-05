#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gfx/Image.h"

#include <stdexcept>

ImageData Image::load(const std::string& path) {
    int w = 0, h = 0, channels = 0;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

    if (!data)
        throw std::runtime_error("Image: cannot load " + path +
                                 " (" + stbi_failure_reason() + ")");

    ImageData result;
    result.width  = w;
    result.height = h;
    result.pixels.assign(data, data + static_cast<std::size_t>(w) * h * 4);

    stbi_image_free(data);
    return result;
}