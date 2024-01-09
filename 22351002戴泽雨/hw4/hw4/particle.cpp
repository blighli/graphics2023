#include"particle.h"
#include<iostream>
namespace MyGame {
	auto BasicUpdater::update(Particle& particle, float dt)->void {
		particle.updateLifeTime(dt);
		float minuser = dt / particle.getLifetime();
		if (minuser > 1.0f) minuser = 1.0f;
		glm::vec4 color = particle.getColor();
		color.w = color.w * (1.0f - minuser);
		particle.setColor(color);
		particle.setPosition(particle.getPositon() + particle.getVelocity() * dt);
	}
	auto BasicInitializer::Init(Particle& particle, float lifetime, const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->void {
		particle.setPosition(position);
		particle.setColor(color);
		particle.setVelocity(velocity);
		particle.setLifeTime(lifetime);
	}
	auto BasicInitializer::Generate(float lifetime,const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->Particle {
		Particle pat(lifetime, position, color, velocity);
		//std::cout << lifetime << std::endl;
		return pat;
	}

	auto RadomInitializer::Init(Particle& particle, float lifetime, const glm::vec2& position, const glm::vec4& color, const glm::vec2 velocity)->void {
		particle.setPosition(position);
		particle.setColor(color);
		particle.setVelocity(velocity);
		particle.setLifeTime(lifetime);
	}
	auto RadomInitializer::Generate(float lifetime, const glm::vec2& aposition, const glm::vec4& acolor, const glm::vec2 avelocity)->Particle {
		float random = ((rand() % 100) - 50) / 10.0f;
		float r1 = ((rand() % 100) - 50) / 200.0f;
		float rx = ((rand() % 100) - 50) / 200.0f;
		float ry = ((rand() % 100) - 50) / 200.0f;
		float rColor = 0.5f + ((rand() % 100) / 100.0f);
		glm::vec4 Color = glm::vec4(rColor * acolor.x, rColor * acolor.y, rColor * acolor.z, 0.1f);
		lifetime += lifetime * r1;
		glm::vec2 velocity = glm::vec2{ avelocity.x * (1 + rx),avelocity.y * (1 + ry) };
		Particle pat(lifetime, aposition , Color , velocity);
		//std::cout << lifetime << std::endl;
		return pat;
	}

	auto ParticleGenerator::addParticle(int num)->void {
		if (initializer == nullptr) return;
		for (int i = 0; i < particleNum; i++) {
			if (num == 0) break;
			if (!particles[tag].isAlive()) {
				particles[tag] = initializer->Generate(lifetime, position, color, velocity);
				num--;
				//std::cout << particles[tag].getLifetime()<<std::endl;
				//std::cout << position.x << std::endl;
			}
			tag = (tag + 1) % particleNum;
		}
	}
	auto ParticleGenerator::update(float dt)->void {
		if (updater == nullptr) return;
		for (auto& item : particles) {
			updater->update(item, dt);
		}
	}
}