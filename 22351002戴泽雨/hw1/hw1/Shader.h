#pragma once
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
		auto set_bool() const -> void;
		auto set_int() const -> void;
		auto set_float() const -> void;
	private:
		struct Impl;
		std::unique_ptr<Impl> impl;
	};
}