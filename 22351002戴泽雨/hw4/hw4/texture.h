#pragma once
#include<glad/glad.h>

namespace Resource {
	using uint = unsigned int;
	using uchar = unsigned char;
	struct Texture2D {
		uint ID;
		uint Width, Height;
		uint Internal_Format; // Format of texture object
		uint Image_Format; // Format of loaded image
		uint Wrap_S;
		uint Wrap_T;
		uint Filter_Min;
		uint Filter_Max;
		Texture2D():ID(-1),Width(0),Height(0),Internal_Format(GL_RGB),Image_Format(GL_RGB),
		Wrap_S(GL_REPEAT),Wrap_T(GL_REPEAT),Filter_Min(GL_LINEAR),Filter_Max(GL_LINEAR){
			glGenTextures(1, &ID); 
		}
		auto Generate(uint width, uint height, const uchar* data)->void;
		auto Bind()->void const;
	private:
	};


}