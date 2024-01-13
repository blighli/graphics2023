#include"texture.h"

namespace Resource {
	auto Texture2D::Generate(uint width, uint height, const uchar* data)->void {
		this->Height = height;
		this->Width = width;
		//create texture;
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, width, height, 0, Image_Format,GL_UNSIGNED_BYTE,data);
		//parameter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,Filter_Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Max);
		//unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	auto Texture2D::Bind()->void const {
		glBindTexture(GL_TEXTURE_2D, ID);
	}

}