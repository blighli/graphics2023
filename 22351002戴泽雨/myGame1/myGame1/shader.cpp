#include"shader.h"
#include<glad/glad.h>
#include<glm/gtc/type_ptr.hpp>
namespace Resource {
	inline auto Shader::Use()->Shader& {
		glUseProgram(ID);
		return (*this);
	}
	auto Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)->void {
		unsigned int sVertex, sFragment, gShader;
		//v
		sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vertexSource, NULL);
		glCompileShader(sVertex);
		checkCompileErrors(sVertex, "VERTEX");

		//f
		sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fragmentSource, NULL);
		glCompileShader(sFragment);
		checkCompileErrors(sFragment, "FRAGMENT");

		//g
		if (geometrySource != nullptr) {
			gShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(gShader, 1, &geometrySource, NULL);
			glCompileShader(gShader);
			checkCompileErrors(gShader, "GEOMETRY");
		}

		//program
		ID = glCreateProgram();
		glAttachShader(ID, sVertex);
		glAttachShader(ID, sFragment);
		if (geometrySource != nullptr) {
			glAttachShader(ID, gShader);
		}

		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
		if (geometrySource != nullptr) {
			glDeleteShader(gShader);
		}

	}
	auto Shader::setFloat(const char* name, float value, bool useShader)->void {
		if (useShader)
			this->Use();
		glUniform1f(glGetUniformLocation(ID, name), value);
	}
	auto Shader::SetInteger(const char* name, int value, bool useShader)->void {
		if (useShader)
			this->Use();
		glUniform1i(glGetUniformLocation(this->ID, name), value);
	}
	auto Shader::SetVector2f(const char* name, float x, float y, bool useShader)->void {
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), x, y);
	}
	auto Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)->void {
		if (useShader)
			this->Use();
		glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
	}
	auto Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)->void{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
	}
	auto Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)->void{
		if (useShader)
			this->Use();
		glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
	}
	auto Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)->void{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
	}
	auto Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)->void{
		if (useShader)
			this->Use();
		glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
	}
	auto Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)->void{
		if (useShader)
			this->Use();
		glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
	}


	auto Shader::checkCompileErrors(unsigned int object, std::string type)->void {
		int success;
		char infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(object, GL_COMPILE_STATUS,&success);
			if (!success) {
				glGetShaderInfoLog(object, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER:Compile error:Type: " << type << std::endl
					<< infoLog << "\n ------------------------------------------------\n";
			}
		}
		else {
			glGetProgramiv(object, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(object, 1024, NULL, infoLog);
				std::cout<<"ERROR::SHADER:Link error:Type: "<<type<<std::endl
					<< infoLog << "\n ------------------------------------------------\n";
			}

		}
	}
}