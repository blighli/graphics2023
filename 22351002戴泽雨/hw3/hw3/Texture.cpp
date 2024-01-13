#include "Texture.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
namespace core::resource
{
	namespace
	{
		auto prefix = std::string{ "./assets/image/" };

		auto add_prefix(const std::string& str) -> std::string
		{
			return prefix + str;
		}
	}



	Texture::Texture(const std::string& path, const std::string& in_kind, bool default_prefix):
		kind{in_kind}
	{
		auto image_path = default_prefix ? add_prefix(path) : path;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		Image image;
		image.data = stbi_load(image_path.c_str(), &image.width, &image.height, &image.channels, 0);
		if (image.data)
		{
			width = image.width;
			height = image.height;
			channels = image.channels;
			switch (image.channels) {
				case 1: {
					type = Texture_Type::r;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_BYTE, image.data);
					glGenerateMipmap(GL_TEXTURE_2D);
					break;
				}

				case 2: {
					type = Texture_Type::rg;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, image.data);
					glGenerateMipmap(GL_TEXTURE_2D);
					break;
				}

				case 3: {
					type = Texture_Type::rgb;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
					glGenerateMipmap(GL_TEXTURE_2D);
					break;
				}

				case 4: {
					type = Texture_Type::rgba;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
					glGenerateMipmap(GL_TEXTURE_2D);
					break;
				}

				default: {
					std::cout << "Error image type\n";
					break;
				}
			}
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
			std::cout << path << std::endl;
		}
		stbi_image_free(image.data);
	}

	auto Texture::bind() const noexcept -> void
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
}