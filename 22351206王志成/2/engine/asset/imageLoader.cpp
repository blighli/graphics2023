#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "imageLoader.hpp"

ImageDescriptor ImageLoader::loadImage(const char *filePath)
{
    ImageDescriptor res{};
    int texChannels;
    auto data = stbi_load(filePath, &res.width, &res.height, &texChannels, 0);
    res.size = res.width * res.height * texChannels;
    res.data = new unsigned char[res.size];
    memcpy(res.data, data, res.size);
    switch (texChannels)
    {
    case 4:
        res.format = vk::Format::eR8G8B8A8Srgb;
        break;
    case 3:
        res.format = vk::Format::eR8G8B8Snorm;
        break;
    case 2:
        res.format = vk::Format::eR8G8Srgb;
        break;
    case 1:
        res.format = vk::Format::eR8Srgb;
        break;
    }
    stbi_image_free(data);
    return res;
}

ImageLoader::ImageLoader()
{
    stbi_set_flip_vertically_on_load(true);
}