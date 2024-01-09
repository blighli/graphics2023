#include"sprite_renderer.h"
#include<glm/glm.hpp>
#include<glm/gtx/transform.hpp>
#include<glad/glad.h>
#include"resource_manager.h"
#include"game_obj.h"
#include"particle.h"

/*#include<iostream>
void printmat(glm::mat4 mat) {
	std::cout << "start print------\n";
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << mat[j][i] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "end of print-----\n\n";
}*/

namespace MyGame {
	//this will change the shader's model matrix and spriteColor vector
	auto SpriteRenderer::drawSprite(Resource::Texture2D* texture,glm::vec2 position, glm::vec2 size , float rotate ,glm::vec4 color)->void {

		glm::mat4 model{ 1.0f };
		model = glm::translate(model, glm::vec3{ position,0.f });
		model = glm::translate(model, glm::vec3{ size.x / 2,size.y / 2,0.f });
		model = glm::rotate(model, glm::radians(rotate), glm::vec3{ 0.,0.,1. });
		model = glm::translate(model, -glm::vec3{ size.x / 2, size.y / 2, 0.f });
		model = glm::scale(model, glm::vec3{ size,0.f });
		shader->Use();
		shader->SetMatrix4("model", model);
		shader->SetVector4f("spriteColor", color);
		//glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		if (texture != nullptr) {
			texture->Bind();
		}
		else {
			std::cout << "no texture\n";
		}
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	auto SpriteRenderer::drawSprite(const GameObj& obj, const glm::vec2& offset)->void {
		drawSprite(obj.getTexture(), obj.getPosition() + offset, obj.getSize(), obj.getAngle(), obj.getColor());
	}

	auto SpriteRenderer::drawSprite(const Particle& particle, const glm::vec2& size, Resource::Texture2D* texture, const glm::vec2& offset)->void {
		drawSprite(texture, particle.getPositon() + offset, size, 0.f, particle.getColor());
	}

	SpriteRenderer::SpriteRenderer():shader(nullptr),VAO(-1) {
		using uint = unsigned int;
		uint VBO;
		const float vertices[] = {
			//pos       //texcoord
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f
		};
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), static_cast<const void*>(vertices), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<const void*>(sizeof(float) * 0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<const void*>(sizeof(float) * 2));
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
		Resource::ResourceManager& r = Resource::getResourceManager();
		shader = r.GetShader("sprite");
		if (shader == nullptr) {
			r.LoadShader("./res/shaders/spritev.hlsl", "./res/shaders/spritef.hlsl", nullptr, "sprite");
			shader = r.GetShader("sprite");
		}
	}
}