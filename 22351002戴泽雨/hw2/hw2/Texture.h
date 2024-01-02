#pragma once
#include <iostream>
#include <unordered_map>
namespace core::resource
{
	struct Image
	{
		int width;
		int height;
		int	channels;
		unsigned char* data{ nullptr };
	};

	struct Texture
	{
		enum struct Texture_Type
		{
			r,
			rg,
			rgb,
			rgba
		};
		Texture_Type type{ Texture_Type::rgb };
		int width{ 0 }, height{ 0 };
		int channels{ 0 };
		Texture(const std::string& path, bool default_prefix = true);
		auto bind() const noexcept -> void;

	private:
		unsigned int texture;
	};

	struct Text_Manager
	{
		static auto instance() -> Text_Manager*
		{
			static auto manager = Text_Manager{};
			return &manager;
		}
		Text_Manager()
		{}
		Text_Manager(const Text_Manager&) = delete;
		Text_Manager(const Text_Manager&&) = delete;

		auto load_texture(const std::string& tex_name, const std::string& path, bool default_prefix = true) noexcept -> void
		{
			if (tex_name == "_None") return;
			if (texture_map.find(tex_name) == texture_map.end()) {
				texture_map[tex_name] = new Texture{path, default_prefix};
			}
		}

		auto get_texture(const std::string& tex_name) -> const Texture*
		{
			if (tex_name == "_None") return nullptr;
			if (texture_map.find(tex_name) == texture_map.end()) {
				return nullptr;
			} else {
				return texture_map[tex_name];
			}
		}

		

	private:
		
		std::unordered_map<std::string, Texture*> texture_map{};
	};
}