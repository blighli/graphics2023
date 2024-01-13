#include"resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "stb_image.h"
#include"glad/glad.h"

namespace Resource {
	template<typename T>
	std::string myItoS(const T& num) {
		std::string ans = "";
		if (num == 0)
		{
			ans = "0";
			return ans;
		}
		T n = num;
		while (n > 0) {
			T a = n % 10;
			ans = static_cast<char>('0' + a) + ans;
			n /= 10;
		}
		return ans;
	}
	auto ResourceManager::LoadShader(const char* vFilename, const char* fFilename, const char* gFilename, const std::string& name)->void {
		Shaders[name] = Shader();
		std::string vCode, fCode, gCode;
		try {
			//open files
			std::fstream vFile{ vFilename }, fFile{ fFilename };
			std::stringstream vStream, fStream;
			// read file's buffer contents into streams
			vStream << vFile.rdbuf();
			fStream << fFile.rdbuf();
			// close file handlers
			vFile.close();
			fFile.close();
			// convert stream into string
			vCode = vStream.str();
			fCode = fStream.str();
			if (gFilename != nullptr) {
				std::fstream gFile{ gFilename };
				std::stringstream gStream;
				gStream << gFile.rdbuf();
				gFile.close();
				gCode = gStream.str();
			}

		}
		catch (std::exception e) {
			std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
		}
		const char* vCode_c = vCode.c_str(), *fCode_c = fCode.c_str();
		const char* gCode_c = nullptr;
		if (gFilename != nullptr)
			gCode_c = gCode.c_str();
		Shaders[name].Compile(vCode_c, fCode_c, gCode_c);
	};
	auto ResourceManager::LoadTexture2D(const char* file, bool alpha, const std::string& name)->void {
		Texture2D& texture = Texture2Ds[name] = Texture2D();
		if (alpha) {
			texture.Internal_Format = GL_RGBA;
			texture.Image_Format = GL_RGBA;
		}
		//load image
		int width, height, nrChannels;
		uchar* data = stbi_load(file, &width, &height, &nrChannels, 0);
		if (!data) {
			std::cout << "no such file: " << file << std::endl;
		}
		texture.Generate(width, height, data);
		stbi_image_free(data);
	}
	auto ResourceManager::LoadTexture2Dmulti(const char* file, bool alpha, const std::string& name, uint hcut, uint wcut)->void {
		
		//load image
		int width, height, nrChannels;
		uchar* data = stbi_load(file, &width, &height, &nrChannels, 0);
		if (!data) {
			std::cout << "no such file: " << file << std::endl;
		}
		std::vector<std::string> mdata (hcut * wcut);
		int mwidth = width / wcut, mheight = height / hcut;
		//i*width+j
		//std::cout << height << " " << width <<" "<<nrChannels<< std::endl;
	

		for (auto& item : mdata)
			item = "";
		//std::cout <<"hehe"<< uint(data[height*width*nrChannels*2]) << std::endl;
			for (uint i = 0; i < mheight*hcut; i++) {
				for (uint j = 0; j < mwidth*wcut; j++) {
					for (uint channel = 0; channel < nrChannels; channel++) {
					//std::cout << "111\n";
					uint itag = i / mheight;
					uint jtag = j / mwidth;
					//std::cout << itag << " " << jtag << std::endl;
					//std::cout << itag <<" "<< jtag<<" "<< channel * width * height + i * width + j << std::endl;
					mdata[itag * wcut + jtag] += data[i * width * nrChannels + j * nrChannels + channel];
				}
			}
		}
		
		for (uint i = 0; i < hcut * wcut; i++) {
			//std::cout << name + myItoS(i) << std::endl;
			Texture2D& texture = Texture2Ds[name + myItoS(i)] = Texture2D();
			if (alpha) {
				texture.Internal_Format = GL_RGBA;
				texture.Image_Format = GL_RGBA;
			}
			texture.Generate(mwidth, mheight, reinterpret_cast<const uchar*>(mdata[i].c_str()));
		}
		stbi_image_free(data);
	};

}