#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// program ID
	unsigned int ID;

	// shader init
	Shader(const char* vertexShaderSourceFilePath, const char* fragmentShaderSourceFilePath);

	// use program
	void use();

	// uniform tools
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

private:
	// utility function for checking shader compilation/linking errors.
	void checkShaderErrors(unsigned int shader, std::string type);

};


#endif // !SHADER_H
