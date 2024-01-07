#pragma once

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Texture
{
public:
    Texture(std::string_view path): textureID(NULL), texturePath(path), data(NULL), width(0), height(0)
    {   
        // 若贴图颠倒，解注释此项
        // stbi_set_flip_vertically_on_load(true);
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // 为当前绑定的纹理对象设置环绕、过滤方式，其中 ST 为纹理坐标轴
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 缩小时有锯齿颗粒，能清晰看到组成纹理的像素，此外还有多级渐远纹理过滤（只能在缩小时使用）
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 线性插值能产生更平滑的图案

        data = stbi_load(texturePath.data(), &width, &height, &nrChannels, 0);// ���ĸ�������ʾchannels��pngͼƬ��4ͨ��rgba��Ϊ4 
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
    }

	~Texture(){	glDeleteTextures(1, &textureID); }

    void Bind(unsigned int slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + slot); 
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

private:
	unsigned int textureID;
	std::string_view texturePath;
	unsigned char* data;
	int width, height, nrChannels;// byte per pixel
};
