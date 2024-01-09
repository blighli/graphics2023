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
		std::string kind{"none"};
		Texture_Type type{ Texture_Type::rgb };
		int width{ 0 }, height{ 0 };
		int channels{ 0 };
		Texture(const std::string& path, const std::string& in_kind = "texture_diffuse", bool default_prefix = true);
		auto bind() const noexcept -> void;

	private:
		unsigned int texture;
	};

	struct Tex_Manager
	{
		static auto instance() -> Tex_Manager*
		{
			static auto manager = Tex_Manager{};
			return &manager;
		}
		Tex_Manager()
		{}
		Tex_Manager(const Tex_Manager&) = delete;
		Tex_Manager(const Tex_Manager&&) = delete;

		auto load_texture(const std::string& tex_name, const std::string& path, const std::string& kind = "texture_diffuse", bool default_prefix = true) noexcept -> const Texture*
		{
			if (tex_name == "_None") return nullptr;
			if (texture_map.find(tex_name) == texture_map.end()) {
				texture_map[tex_name] = new Texture{path, kind, default_prefix};
				return texture_map[tex_name];
			}
			return nullptr;
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