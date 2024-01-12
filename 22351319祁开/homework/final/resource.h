#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "ecs.h"


#include"utils.h"
#include <vector>
#include <array>
#include <unordered_map>
#include<string>
#include<chrono>
struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//vec3
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;//vec3
		attributeDescriptions[1].offset = offsetof(Vertex, normal);
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
	bool operator==(const Vertex& other) const {
		return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
	}
};
namespace std {
	template<> struct hash<Vertex> {
		inline size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct Object
{
	glm::mat4 model = glm::mat4(1.);
	glm::mat4 scale = glm::mat4(1.);
	float r;
	glm::vec3 location = glm::vec3(0.);
	glm::mat4 rotation = glm::mat4(1.);
	std::string texture_path;
	std::uint32_t texture_index;
	std::string name;
	Object() {
	}
	void setR(float r) {
		this->r = r;
		scale = glm::scale(glm::mat4(1.), glm::vec3(r, r, r));
	}
	glm::mat4& calculate() {
		model = glm::translate(glm::mat4(1.0f), location) * rotation * scale;
		return model;
	}
};
inline std::vector<Object> objects;
inline void initObjects() {
	const int objects_num=10001;
	for (size_t i = 0; i < objects_num; i++) {

		Object obj;
		obj.name = u8"Ä£ÐÍ"u8;
		int x = i % 100;
		int y= i / 100;
		obj.location = glm::vec3(x, 0, y);
		obj.rotation = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 2 * 3, glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 2 * 3, glm::vec3(1.0f, 0.0f, 0.0f));
		obj.texture_path = "textures/viking_room.png";
		obj.setR(0.1);
		if(i==0)obj.setR(0.2);
		objects.push_back(obj);
	}
	
}
inline void updateObjects() {

	for (size_t i = 0; i < objects.size(); ++i) {
		
		objects[i].calculate();
	}

}



class Engine;
class Resource {
public:
	Resource(Engine& engine);
	void init();
	void update();
private:
	Engine& engine;
	ecs::World ecsWorld;
};
