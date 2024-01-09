#pragma once
#include"shader.h"
#include"texture.h"
#include<glm/glm.hpp>

namespace MyGame {
	struct GameObj;
	struct Particle;
	struct SpriteRenderer {
		auto drawSprite(Resource::Texture2D* texture,glm::vec2 position, glm::vec2 size = glm::vec2{ 10,10 }, float rotate = 0.f,
			glm::vec4 color = glm::vec4{ 1.0f })->void;
		auto drawSprite(const GameObj&, const glm::vec2 &offset = glm::vec2{0.f})->void;
		auto drawSprite(const Particle&, const glm::vec2& size = glm::vec2{ 2.0f }, Resource::Texture2D* texture = nullptr, const glm::vec2& offset = glm::vec2{ 0.f })->void;
		static auto getInstance()->SpriteRenderer& {
			static SpriteRenderer r{};
			return r;
		}
		
	private:
		unsigned int VAO;
		Resource::Shader* shader;
		SpriteRenderer();
		~SpriteRenderer() {}
		SpriteRenderer(const SpriteRenderer& r) {}
		SpriteRenderer(const SpriteRenderer&& r) {}
		auto operator = (const SpriteRenderer& r)->SpriteRenderer& {}
	};
	static auto getSpriteRenderer()->SpriteRenderer& {
		return SpriteRenderer::getInstance();
	}
}