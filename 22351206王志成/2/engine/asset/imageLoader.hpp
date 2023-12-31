#pragma once

// #include <FreeImage.h>
#include <vulkan/vulkan.hpp>

struct ImageDescriptor
{
    int32_t width{};
    int32_t height{};
    vk::DeviceSize size{};
    vk::Format format{};
    unsigned char *data{nullptr};
};

// class ImageLoader
// {
// public:
//     ImageLoader(const ImageLoader &) = delete;
//     ImageLoader(ImageLoader &&) = delete;
//     ImageLoader &operator=(const ImageLoader &) = delete;
//     ImageLoader &operator=(ImageLoader &&) = delete;

//     static ImageLoader &getInstance()
//     {
//         static ImageLoader instance;
//         return instance;
//     }

//     auto loadImage(const char *filePath)
//     {
//         auto fif = FreeImage_GetFileType(filePath);
//         if (fif == FIF_UNKNOWN)
//             fif = FreeImage_GetFIFFromFilename(filePath);
//         if (fif == FIF_UNKNOWN)
//         {
//             printf("image has unknown file type, and it is failed to load.\n");
//             return ImageDescriptor{};
//         }
//         if (!FreeImage_FIFSupportsReading(fif))
//         {
//             printf("unsupported image reading type.\n");
//             return ImageDescriptor{};
//         }
//         auto content = FreeImage_Load(fif, filePath, 0);
//         if (content != nullptr)
//         {
//             ImageDescriptor res{};
//             res.width = FreeImage_GetWidth(content);
//             res.height = FreeImage_GetHeight(content);
//             res.data = new unsigned char[res.width * res.height * FreeImage_GetBPP(content)];
//             memcpy(res.data, FreeImage_GetBits(content), res.width * res.height * FreeImage_GetBPP(content));
//             auto type = FreeImage_GetFileType(reinterpret_cast<const char *>(content->data));
//             switch (type)
//             {
//             case FIT_UINT16:
//                 res.format = vk::Format::eR16Uint;
//                 break;
//             case FIT_INT16:
//                 res.format = vk::Format::eR16Sint;
//                 break;
//             case FIT_UINT32:
//                 res.format = vk::Format::eR32Uint;
//                 break;
//             case FIT_INT32:
//                 res.format = vk::Format::eR32Sint;
//                 break;
//             case FIT_FLOAT:
//                 res.format = vk::Format::eR32Sfloat;
//                 break;
//             case FIT_DOUBLE:
//                 res.format = vk::Format::eR64Sfloat;
//                 break;
//             case FIT_COMPLEX:
//                 res.format = vk::Format::eR64G64Sfloat;
//                 break;
//             case FIT_RGB16:
//                 res.format = vk::Format::eR16G16B16Uint;
//                 break;
//             case FIT_RGBA16:
//                 res.format = vk::Format::eR16G16B16A16Uint;
//                 break;
//             case FIT_RGBF:
//                 res.format = vk::Format::eR32G32B32Sfloat;
//                 break;
//             case FIT_RGBAF:
//                 res.format = vk::Format::eR32G32B32A32Sfloat;
//                 break;
//             case FIT_BITMAP:
//             {
//                 auto bpp = FreeImage_GetBPP(content);
//                 switch (bpp)
//                 {
//                 case 32:
//                     res.format = vk::Format::eR8G8B8A8Srgb;
//                     break;
//                 case 24:
//                     res.format = vk::Format::eR8G8B8Srgb;
//                     break;
//                 case 16:
//                     res.format = vk::Format::eR8G8Srgb;
//                     break;
//                 case 8:
//                     res.format = vk::Format::eR8Srgb;
//                     break;
//                 default:
//                     printf("detected standard bitmap, but it has BPP less than 4 and cannot be read.\n");
//                     FreeImage_Unload(content);
//                     return ImageDescriptor{};
//                 }
//             }
//             break;
//             default:
//                 printf("unknown image format.\n");
//                 FreeImage_Unload(content);
//                 return ImageDescriptor{};
//             }

//             FreeImage_Unload(content);
//             return res;
//         }
//         return ImageDescriptor{};
//     }

// private:
//     ImageLoader()
//     {
//         FreeImage_Initialise(TRUE);
//     }

//     ~ImageLoader()
//     {
//         FreeImage_DeInitialise();
//     }
// };

class ImageLoader
{
public:
    ImageLoader(const ImageLoader &) = delete;
    ImageLoader(ImageLoader &&) = delete;
    ImageLoader &operator=(const ImageLoader &) = delete;
    ImageLoader &operator=(ImageLoader &&) = delete;

    static ImageLoader &getInstance()
    {
        static ImageLoader instance;
        return instance;
    }

    ImageDescriptor loadImage(const char *filePath);

private:
    ImageLoader();

    ~ImageLoader() {}
};