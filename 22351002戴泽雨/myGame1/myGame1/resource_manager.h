#pragma once
#include<map>
#include"shader.h"
#include"texture.h"
//resource manager is not a file system, it only manage the resources
//do not control IO
namespace Resource {
	//resourceManager是一个单例
	using uchar = unsigned char;
	using uint = unsigned int;
	struct ResourceManager {
		auto LoadShader(const char* vFilename, const char* fFilename, const char* gFilename, const std::string& name)->void;
		auto GetShader(const std::string& name,bool *isExest=nullptr)->Shader* {
			auto iter = Shaders.find(name);
			if (iter != Shaders.end()) {
				return &iter->second;
			}
			else {
				return nullptr;
			}
		};
		auto LoadTexture2D(const char* file, bool alpha, const std::string& name)->void;
		auto LoadTexture2Dmulti(const char* file, bool alpha, const std::string& name, uint xcut = 0, uint ycut = 0)->void;
		auto GetTexture2D(const std::string& name)->Texture2D* {
			auto iter = Texture2Ds.find(name);
			if (iter != Texture2Ds.end()) {
				return &iter->second;
			}
			else {
				std::cout << "no texture in resources : "<<name<<std::endl;
				return nullptr;
			}
		};
		auto Clear()->void;
		static auto getInstance()->ResourceManager&{
			static ResourceManager r{};
			return r;
		}
		
	private:
		ResourceManager() {}
		~ResourceManager() {}
		ResourceManager(const ResourceManager& r){}
		ResourceManager(const ResourceManager&& r) noexcept{}
		auto operator = (const ResourceManager& r)->ResourceManager& {}
		std::map<std::string, Shader> Shaders;
		std::map<std::string, Texture2D> Texture2Ds;
	};
	static auto getResourceManager()->ResourceManager& {
		return ResourceManager::getInstance();
	}
	
}