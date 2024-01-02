#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>

namespace core::shader
{
	namespace
	{
		auto prefix = std::string{ "./shader/" };
	}
	
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

	auto Shader::set_bool(const std::string&name, bool value) const -> void
	{
		glUniform1i(glGetUniformLocation(impl->program_id, name.c_str()), static_cast<int>(value));
	}

	auto Shader::set_int(const std::string& name, int value) const -> void
	{
		glUniform1i(glGetUniformLocation(impl->program_id, name.c_str()), value);
	}

	auto Shader::set_float(const std::string& name, float value) const -> void
	{
		glUniform1i(glGetUniformLocation(impl->program_id, name.c_str()), value);
	}

	auto Shader::set_vec2(const std::string& name, const glm::vec2& value) const -> void
	{
		glUniform2fv(glGetUniformLocation(impl->program_id, name.c_str()), 1, &value[0]);
	}

	auto Shader::set_vec2(const std::string& name, float x, float y) const -> void
	{
		glUniform2f(glGetUniformLocation(impl->program_id, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	auto Shader::set_vec3(const std::string& name, const glm::vec3& value) const -> void
	{
		glUniform3fv(glGetUniformLocation(impl->program_id, name.c_str()), 1, &value[0]);
	}
	auto Shader::set_vec3(const std::string& name, float x, float y, float z) const -> void
	{
		glUniform3f(glGetUniformLocation(impl->program_id, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	auto Shader::set_vec4(const std::string& name, const glm::vec4& value) const -> void
	{
		glUniform4fv(glGetUniformLocation(impl->program_id, name.c_str()), 1, &value[0]);
	}
	auto Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const -> void
	{
		glUniform4f(glGetUniformLocation(impl->program_id, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	auto Shader::set_mat2(const std::string& name, const glm::mat2& mat) const -> void
	{
		glUniformMatrix2fv(glGetUniformLocation(impl->program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	auto Shader::set_mat3(const std::string& name, const glm::mat3& mat) const -> void
	{
		glUniformMatrix3fv(glGetUniformLocation(impl->program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	auto Shader::set_mat4(const std::string& name, const glm::mat4& mat) const -> void
	{
		glUniformMatrix4fv(glGetUniformLocation(impl->program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

}