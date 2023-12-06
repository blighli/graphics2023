#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

namespace core::shader
{
	auto prefix = std::string{"./shader/"};
	
	auto set_shader_path(const std::string& _prefix)
	{
		prefix = _prefix;
	}

	struct Shader::Impl
	{
		GLint program_id;
	};

	Shader::Shader(const std::string& vertex_name, const std::string& fragment_name) : impl{ std::make_unique<Impl>() }
	{
		auto vertex_code = std::string{};
		auto frag_code = std::string{};
		auto vertex_file = std::ifstream{};
		auto frag_file = std::ifstream{};
		auto vertex_path = prefix + vertex_name;
		auto fragment_path = prefix + fragment_name;
		vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		frag_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			vertex_file.open(vertex_path);
			frag_file.open(fragment_path);
			std::stringstream vertex_stream, frag_stream;
			vertex_stream << vertex_file.rdbuf();
			frag_stream << frag_file.rdbuf();
			vertex_code = vertex_stream.str();
			frag_code = frag_stream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::Filed to read file." << std::endl;
			std::cout << "Path: " << vertex_path << std::endl << fragment_path << std::endl;
		}
		
		auto vertex_code_ptr = vertex_code.c_str();
		auto frag_code_ptr = frag_code.c_str();
		
		// program shader
		auto success = int{};
		char info_log[512];
		
		// vertex shader
		auto vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertex_code_ptr, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, info_log);
			std::cout << "ERROR::SHADER::VERTEX::Compilation failed.\n" << info_log << std::endl;
		}
		
		// fragment shader
		auto frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &frag_code_ptr, NULL);
		glCompileShader(frag);
		glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, info_log);
			std::cout << "ERROR::SHADER::FRAGMENT::Compilation failed.\n" << info_log << std::endl;
		}
		
		// link shader
		impl->program_id = glCreateProgram();
		glAttachShader(impl->program_id, vertex);
		glAttachShader(impl->program_id, frag);
		glLinkProgram(impl->program_id);
		glGetProgramiv(impl->program_id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(impl->program_id, 512, NULL, info_log);
			std::cout << "ERROR::SHADER::PROGRAM::Link failed.\n" << info_log << std::endl;
		}
		
		glDeleteShader(vertex);
		glDeleteShader(frag);
	}

	Shader::~Shader() = default;

	auto Shader::use() const -> void
	{
		glUseProgram(impl->program_id);
	}
}