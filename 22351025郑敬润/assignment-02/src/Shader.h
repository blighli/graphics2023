#pragma once

#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件
#include <glm/glm.hpp>

// Standard Headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 着色器构造函数：读取并构建着色器，这里使用C++17 string_view特性
	Shader(std::string_view vertexPath, std::string_view fragmentPath){
        //--------------------------------- 1. 从文件路径中获取顶点/片段着色器---------------------------------//
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // 保证ifstream对象可以抛出异常：
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // 打开文件
            vShaderFile.open(vertexPath.data());
            fShaderFile.open(fragmentPath.data());
            std::stringstream vShaderStream, fShaderStream;
            // 读取文件的缓冲内容到数据流中
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // 关闭文件处理器
            vShaderFile.close();
            fShaderFile.close();
            // 转换数据流到string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        // 将 string 类型的字符串转化为 char 数组类型
        const char* vShaderCode = vertexCode.data();
        const char* fShaderCode = fragmentCode.data();
        //--------------------------------- 1. 从文件路径中获取顶点/片段着色器---------------------------------//

        //--------------------------------- 2. 编译着色器---------------------------------//
        // 顶点着色器
        unsigned int vertex;
        vertex = glCreateShader(GL_VERTEX_SHADER); // 创建一个着色器对象，把需要创建的着色器类型以参数形式提供给glCreateShader，传递的参数是GL_VERTEX_SHADER
        glShaderSource(vertex, 1, &vShaderCode, NULL); // 把这个着色器源码附加到着色器对象上，然后编译它
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX"); // 打印编译错误（如果有的话）

        // 片段编译器，同上
        unsigned int fragment;
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // 着色器程序
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }


    // 使用/激活程序
    void use()
	{
		glUseProgram(ID);
	}
	void Bind() const
	{
		glUseProgram(ID);
	}
	// 解绑程序
	void Unbind() const
	{
		glUseProgram(0);
	}
	// ----------------------------------uniform工具函数--------------------------------------
	// C++17 string_view特性使用：可以直接输入一个char字符串数组，如myShader.setInt("texture2", 1);
	void setBool(std::string_view name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.data()), (int)value);
	}
	void setInt(std::string_view name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.data()), value);
	}
	void setFloat(std::string_view name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.data()), value);
	}
	// ----------------------------------使用glm库-------------------------------------
	void setVec2(std::string_view name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.data()), 1, &value[0]);
	}
	void setVec2(std::string_view name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.data()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(std::string_view name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.data()), 1, &value[0]);
	}
	void setVec3(std::string_view name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.data()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(std::string_view name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.data()), 1, &value[0]);
	}
	void setVec4(std::string_view name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.data()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(std::string_view name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(std::string_view name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(std::string_view name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, &mat[0][0]);
	}
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};