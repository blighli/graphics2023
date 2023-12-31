#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
namespace core::shader
{
	auto set_shader_path(const std::string& prefix);
	struct Shader
	{
		Shader(const std::string& vertex_name, const std::string& fragment_name);
		~Shader();
		//auto reload(std::string vertex_path, std::string fragment_path);
		auto use() const -> void;
		auto set_bool(const std::string&, bool) const -> void;
		auto set_int(const std::string&, int) const -> void;
		auto set_float(const std::string&, float) const -> void;
		auto set_vec2(const std::string& name, const glm::vec2& value) const -> void;
		auto set_vec2(const std::string& name, float x, float y) const -> void;
		auto set_vec3(const std::string& name, const glm::vec3& value) const -> void;
        auto set_vec3(const std::string& name, float x, float y, float z) const -> void;
        auto set_vec4(const std::string& name, const glm::vec4& value) const -> void;
        auto set_vec4(const std::string& name, float x, float y, float z, float w) const -> void;
        auto set_mat2(const std::string& name, const glm::mat2& mat) const -> void;
        auto set_mat3(const std::string& name, const glm::mat3& mat) const -> void;
        auto set_mat4(const std::string& name, const glm::mat4& mat) const -> void;
	
	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
	};
}